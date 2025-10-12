#ifndef __POINTER_LIST_H
#define __POINTER_LIST_H

#include <stdlib.h>
#include <string.h>
#include <map>
#include <atomic>

#ifndef INT_POINTER_TYPE
	#define INT_POINTER_TYPE	unsigned int
#endif

#define DEFINE_LIST(list)			struct pointer_list list
#define MAP_POINTER(list, value, context)	pointerList_add(&list, value, context)
#define GET_POINTER(list, value, context)	pointerList_pointer(&list, value, context)
#define FREE_POINTER(list, value, context)	pointerList_free(&list, value, context)
#define INIT_LIST(list)				pointerList_init(&list)
#define DEINIT_LIST(list)			pointerList_deinit(&list)

class pointer_list_object {
public:
	const void *ptr;
	const void *context;
	INT_POINTER_TYPE ptr_id;

	pointer_list_object(const void *ptr = NULL, const void *context = NULL, INT_POINTER_TYPE ptr_id = 0) {
		this->ptr = ptr;
		this->context = context;
		this->ptr_id = ptr_id;
	}
};

struct pointer_list {
	std::map<INT_POINTER_TYPE, pointer_list_object> map;
	std::atomic_flag atomic_flag = ATOMIC_FLAG_INIT;
	INT_POINTER_TYPE index;
	INT_POINTER_TYPE reuse_index;
};

void pointerList_init(struct pointer_list *ptr_list) {

	if (!ptr_list)
		return;

	ptr_list->index = 0;
	ptr_list->reuse_index = 0;
}

INT_POINTER_TYPE pointerList_add(struct pointer_list *ptr_list, const void *ptr, const void *context) {
	if ((!ptr_list) || (!ptr))
		return 0;

	while (ptr_list->atomic_flag.test_and_set(std::memory_order_acquire)) { };

	INT_POINTER_TYPE index = ptr_list->reuse_index;
	if (index) {
		ptr_list->reuse_index = 0;
	} else {
		index = ++ ptr_list->index;
	}
	pointer_list_object ptr_2(ptr, context, index);
	ptr_list->map[index] = ptr_2;

	ptr_list->atomic_flag.clear(std::memory_order_release);

	return index;
}

void *pointerList_pointer(struct pointer_list *ptr_list, INT_POINTER_TYPE ptr_id, const void *context) {

	if ((!ptr_id) || (!ptr_list))
		return (void *)0;

	while (ptr_list->atomic_flag.test_and_set(std::memory_order_acquire)) { };
	pointer_list_object ptr_2 = ptr_list->map[ptr_id];
	ptr_list->atomic_flag.clear(std::memory_order_release);

	if (ptr_2.context != context)
		return (void *)0;

	return (void *)ptr_2.ptr;
}

void *pointerList_free(struct pointer_list *ptr_list, INT_POINTER_TYPE ptr_id, const void *context) {
	if ((!ptr_id) || (!ptr_list))
		return (void *)0;

	while (ptr_list->atomic_flag.test_and_set(std::memory_order_acquire)) { };

	pointer_list_object ptr_2 = ptr_list->map[ptr_id];
	if (ptr_2.context != context) {
		ptr_list->atomic_flag.clear(std::memory_order_release);
		return (void *)0;
	}

	ptr_list->map.erase(ptr_id);

	if (ptr_id == ptr_list->index)
		ptr_list->index --;
	else
		ptr_list->reuse_index = ptr_id;

	ptr_list->atomic_flag.clear(std::memory_order_release);

	return (void *)ptr_2.ptr;
}

void pointerList_deinit(struct pointer_list *ptr_list) {
	if (!ptr_list)
		return;

	while (ptr_list->atomic_flag.test_and_set(std::memory_order_acquire)) { };
	ptr_list->map.clear();
	ptr_list->atomic_flag.clear(std::memory_order_release);
}

#endif
