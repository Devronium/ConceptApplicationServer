#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "khash.h"
#include "hash64.h"

KHASH_MAP_INIT_INT64(ip_list, uint32_t)

struct firewall_container {
	khash_t(ip_list) *rotation_1_light;
	khash_t(ip_list) *rotation_2_light;
	time_t rotation_light_timestamp;

	khash_t(ip_list) *rotation_1_heavy;
	khash_t(ip_list) *rotation_2_heavy;
	time_t rotation_heavy_timestamp;

	khash_t(ip_list) *always_block;

	khash_t(ip_list) *requests_per_minute_count;
	time_t requests_per_minute_timestamp;

	uint32_t rotation_light_timeout;
	uint32_t rotation_heavy_timeout;
	uint32_t rotation_light_threshold;
	uint32_t rotation_heavy_threshold;
	uint32_t requests_per_minute_threshold;

	uint32_t rotation_light_epoch;
	uint32_t rotation_heavy_epoch;

	uint64_t hash_seed;
};

struct firewall_container *firewall_init(uint32_t rotation_light_timeout, uint32_t rotation_heavy_timeout, uint32_t rotation_light_threshold, uint32_t rotation_heavy_threshold, uint32_t requests_per_minute_threshold) {
	struct firewall_container *container = (struct firewall_container *)malloc(sizeof(struct firewall_container));

	if ((!container) || (!rotation_light_timeout) || (!rotation_heavy_timeout))
		return NULL;

	memset(container, 0, sizeof(struct firewall_container));

	container->rotation_1_light = kh_init(ip_list);
	container->rotation_2_light = kh_init(ip_list);
	container->rotation_1_heavy = kh_init(ip_list);
	container->rotation_2_heavy = kh_init(ip_list);
	container->always_block = kh_init(ip_list);
	container->requests_per_minute_count = kh_init(ip_list);

	container->rotation_light_timeout = rotation_light_timeout;
	container->rotation_heavy_timeout = rotation_heavy_timeout;
	container->rotation_light_threshold = rotation_light_threshold;
	container->rotation_heavy_threshold = rotation_heavy_threshold;
	container->requests_per_minute_threshold = requests_per_minute_threshold;

	container->rotation_light_timestamp = time(NULL) + rotation_light_timeout;
	container->rotation_heavy_timestamp = time(NULL) + rotation_heavy_timeout;
	container->requests_per_minute_timestamp = time(NULL) + 60;

	srand(time(NULL));

	container->hash_seed = rand();

	return container;
}

void firewall_free(struct firewall_container *container) {
	if (!container)
		return;

	if (container->rotation_1_light)
		kh_destroy(ip_list, container->rotation_1_light);

	if (container->rotation_2_light)
		kh_destroy(ip_list, container->rotation_2_light);

	if (container->rotation_1_heavy)
		kh_destroy(ip_list, container->rotation_1_heavy);

	if (container->rotation_2_heavy)
		kh_destroy(ip_list, container->rotation_2_heavy);

	if (container->always_block)
		kh_destroy(ip_list, container->always_block);

	if (container->requests_per_minute_count)
		kh_destroy(ip_list, container->requests_per_minute_count);

	free(container);
}

static khash_t(ip_list) *_firewall_get_light_rotation(struct firewall_container *container, int switch_to_next_epoch_after_half_timeout) {
	int epoch = container->rotation_light_epoch;
	if (switch_to_next_epoch_after_half_timeout) {
		int half = container->rotation_light_timeout / 2;
		if ((half > 0) && (time(NULL) >= (container->rotation_light_timestamp - half)))
			epoch ++;
	}
	if (epoch % 2 == 0)
		return container->rotation_1_light;

	return container->rotation_2_light;
}

static inline uint64_t _firewall_hash(struct firewall_container *container, const char *str) {
	if (!str)
		return hash64(str, 0, container->hash_seed);

	return hash64(str, strlen(str), container->hash_seed);
}

khash_t(ip_list) *_firewall_get_heavy_rotation(struct firewall_container *container, int switch_to_next_epoch_after_half_timeout) {
	int epoch = container->rotation_heavy_epoch;
	if (switch_to_next_epoch_after_half_timeout) {
		int half = container->rotation_heavy_timeout / 2;
		if ((half > 0) && (time(NULL) >= (container->rotation_heavy_timeout - half)))
			epoch ++;
	}
	if (epoch % 2 == 0)
		return container->rotation_1_heavy;

	return container->rotation_2_heavy;
}

static uint64_t _firewall_check(khash_t(ip_list) *epoch, uint64_t ip_hash) {
	khint64_t k = kh_get(ip_list, epoch, ip_hash);

	if (k == kh_end(epoch))
		return 0;

	return (uint64_t)kh_value(epoch, k);
}

void firewall_check_threshold(struct firewall_container *container) {
	if (!container)
		return;

	time_t now = time(NULL);

	if (now >= container->rotation_light_timestamp) {
		khash_t(ip_list) *old_epoch = _firewall_get_light_rotation(container, 0);
		kh_clear(ip_list, old_epoch);

		container->rotation_light_timestamp = time(NULL) + container->rotation_light_timeout;
		container->rotation_light_epoch ++;
	}

	if (now >= container->rotation_heavy_timestamp) {
		khash_t(ip_list) *old_epoch = _firewall_get_heavy_rotation(container, 0);
		kh_clear(ip_list, old_epoch);

		container->rotation_heavy_timestamp = time(NULL) + container->rotation_heavy_timeout;
		container->rotation_heavy_epoch ++;
	}

	if (now >= container->requests_per_minute_timestamp) {
		kh_clear(ip_list, container->requests_per_minute_count);
		container->requests_per_minute_timestamp = time(NULL) + 60;
	}
}

int firewall_is_blocked(struct firewall_container *container, const char *tag) {
	if (!container)
		return -1;

	firewall_check_threshold(container);

	uint64_t ip_hash = _firewall_hash(container, tag);

	uint64_t count = _firewall_check(container->rotation_1_heavy, ip_hash) + _firewall_check(container->rotation_2_heavy, ip_hash);

	if (count >= container->rotation_heavy_threshold)
		return 1;

	count = _firewall_check(container->rotation_1_light, ip_hash) + _firewall_check(container->rotation_2_light, ip_hash);

	if (count >= container->rotation_light_threshold)
		return 1;

	count = _firewall_check(container->requests_per_minute_count, ip_hash);
	if (count >= container->requests_per_minute_threshold)
		return 1;
	
	return _firewall_check(container->always_block, ip_hash);
}

int firewall_light_block(struct firewall_container *container, const char *tag) {
	if (!container)
		return  -1;

	firewall_check_threshold(container);

	uint64_t ip_hash = _firewall_hash(container, tag);

	khash_t(ip_list) *epoch = _firewall_get_light_rotation(container, 1);

	uint64_t count = _firewall_check(epoch, ip_hash);

	if (count >= container->rotation_light_threshold)
		return count;

	count ++;

	int absent;
	khint_t k = kh_put(ip_list, epoch, ip_hash, &absent);

	kh_value(epoch, k) = count;

	return count;
}

int firewall_heavy_block(struct firewall_container *container, const char *tag) {
	if (!container)
		return -1;

	firewall_check_threshold(container);

	uint64_t ip_hash = _firewall_hash(container, tag);

	khash_t(ip_list) *epoch = _firewall_get_heavy_rotation(container, 1);

	uint64_t count = _firewall_check(epoch, ip_hash);

	if (count >= container->rotation_heavy_threshold)
		return count;

	count ++;

	int absent;
	khint_t k = kh_put(ip_list, epoch, ip_hash, &absent);

	kh_value(epoch, k) = count;

	return count;
}

int firewall_always_block(struct firewall_container *container, const char *tag) {
	if (!container)
		return -1;

	firewall_check_threshold(container);

	uint64_t ip_hash = _firewall_hash(container, tag);

	khash_t(ip_list) *epoch = container->always_block;

	uint64_t count = _firewall_check(epoch, ip_hash);

	if (count > 0)
		return count;

	int absent;
	khint_t k = kh_put(ip_list, epoch, ip_hash, &absent);

	kh_value(epoch, k) = 1;

	return 1;
}

int firewall_inc_request_count(struct firewall_container *container, const char *tag) {
	if (!container)
		return 0;

	firewall_check_threshold(container);

	uint64_t ip_hash = _firewall_hash(container, tag);

	uint64_t count = _firewall_check(container->requests_per_minute_count, ip_hash);

	count ++;

	int absent;
	khint_t k = kh_put(ip_list, container->requests_per_minute_count, ip_hash, &absent);

	kh_value(container->requests_per_minute_count, k) = count;

	return (count > container->requests_per_minute_threshold);
}
