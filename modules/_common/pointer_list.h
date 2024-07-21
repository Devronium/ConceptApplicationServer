#ifndef __POINTER_LIST_H
#define __POINTER_LIST_H

#include <stdlib.h>
#include <string.h>

#ifndef INT_POINTER_TYPE
	#define INT_POINTER_TYPE	int
#endif

#ifndef POINTER_WRAP_INDEX
	#define POINTER_WRAP_INDEX	1024
#endif

#ifndef POINTER_LIST_INCREMENT
	#define POINTER_LIST_INCREMENT	10
#endif

#ifndef POINTER_DATA_INCREMENT
	#define POINTER_DATA_INCREMENT	4
#endif

#ifndef POINTER_REUSE_INCREMENT
	#define POINTER_REUSE_INCREMENT	10
#endif

#define MAP_POINTER(list, value, context)	pointerList_add(&list, value, context)
#define GET_POINTER(list, value, context)	pointerList_get(&list, value, context)
#define FREE_POINTER(list, value, context)	pointerList_free(&list, value, context)
#define INIT_LIST(list)				pointerList_init(&list)
#define DEINIT_LIST(list)			pointerList_deinit(&list)

struct pointer_data {
	INT_POINTER_TYPE id;
	const void *ptr;
	const void *context;
};

struct pointer_line {
	struct pointer_data *data;
	INT_POINTER_TYPE data_size;
};

struct pointer_list {
	INT_POINTER_TYPE index;

	struct pointer_line *pointer_list;
	INT_POINTER_TYPE pointer_list_size;

	INT_POINTER_TYPE *reuse_ids;
	INT_POINTER_TYPE reuse_ids_size;
	INT_POINTER_TYPE reuse_ids_start;
	INT_POINTER_TYPE reuse_ids_lookup_start;
};

void pointerList_init(struct pointer_list *ptr_list) {
	if (!ptr_list)
		return;

	memset(ptr_list, 0, sizeof(struct pointer_list));
}

INT_POINTER_TYPE pointerList_add(struct pointer_list *ptr_list, const void *ptr, const void *context) {
	if ((!ptr_list) || (!ptr))
		return 0;

	INT_POINTER_TYPE ptr_id = 0;

	INT_POINTER_TYPE i;
	for (i = ptr_list->reuse_ids_lookup_start; i < ptr_list->reuse_ids_size; i ++) {
		if (ptr_list->reuse_ids[i]) {
			ptr_id = ptr_list->reuse_ids[i];
			ptr_list->reuse_ids[i] = 0;
			ptr_list->reuse_ids_start = i;
			ptr_list->reuse_ids_lookup_start = i;
			break;
		}
	}

	if (!ptr_id) {
		for (i = 0; i < ptr_list->reuse_ids_lookup_start; i ++) {
			if (ptr_list->reuse_ids[i]) {
				ptr_id = ptr_list->reuse_ids[i];
				ptr_list->reuse_ids[i] = 0;
				ptr_list->reuse_ids_start = i;
				ptr_list->reuse_ids_lookup_start = i;
				break;
			}
		}
	}

	if (!ptr_id) {
		if (ptr_list->reuse_ids) {
			free(ptr_list->reuse_ids);
			ptr_list->reuse_ids = (INT_POINTER_TYPE *)0;
			ptr_list->reuse_ids_size = 0;
			ptr_list->reuse_ids_start = 0;
		}
		ptr_id = (ptr_list->index ++);
	} else
		ptr_id --;

	INT_POINTER_TYPE list_index = ptr_id % POINTER_WRAP_INDEX;

	if (ptr_list->pointer_list_size <= list_index) {
		ptr_list->pointer_list_size += POINTER_LIST_INCREMENT;
		struct pointer_line *old_ptr = ptr_list->pointer_list;
		ptr_list->pointer_list = (struct pointer_line *)realloc(ptr_list->pointer_list, ptr_list->pointer_list_size * sizeof(struct pointer_line));
		if (!ptr_list->pointer_list) {
			ptr_list->pointer_list = old_ptr;
			ptr_list->pointer_list_size -= POINTER_LIST_INCREMENT;
			return 0;
		}

		INT_POINTER_TYPE i;
		for (i = ptr_list->pointer_list_size - POINTER_LIST_INCREMENT; i < ptr_list->pointer_list_size; i ++) {
			ptr_list->pointer_list[i].data = (struct pointer_data *)0;
			ptr_list->pointer_list[i].data_size = 0;
		}
	}
	
	ptr_id ++;
	struct pointer_line *ptr_line = &ptr_list->pointer_list[list_index];
	for (i = 0; i < ptr_line->data_size; i ++) {
		if (!ptr_line->data[i].id) {
			ptr_line->data[i].id = ptr_id;
			ptr_line->data[i].ptr = ptr;
			ptr_line->data[i].context = context;
			return ptr_id;
		}
	}

	ptr_line->data_size += POINTER_DATA_INCREMENT;
	struct pointer_data *old_data = ptr_line->data;
	ptr_line->data = (struct pointer_data *)realloc(ptr_line->data, ptr_line->data_size * sizeof(struct pointer_data));
	if (!ptr_line->data) {
		ptr_line->data = old_data;
		ptr_line->data_size -= POINTER_DATA_INCREMENT;
		return 0;	
	}

	for (i = ptr_line->data_size - POINTER_DATA_INCREMENT + 1; i < ptr_line->data_size; i ++) {
		ptr_line->data[i].id = 0;
		ptr_line->data[i].ptr = (void *)0;
		ptr_line->data[i].context = (void *)0;
	}

	i = ptr_line->data_size - POINTER_DATA_INCREMENT;
	ptr_line->data[i].id = ptr_id;
	ptr_line->data[i].ptr = ptr;
	ptr_line->data[i].context = context;

	return ptr_id;
}

void *pointerList_pointer(struct pointer_list *ptr_list, INT_POINTER_TYPE ptr_id, const void *context) {
	if ((ptr_id <= 0) || (!ptr_list))
		return (void *)0;

	
	INT_POINTER_TYPE list_index = (ptr_id - 1) % POINTER_WRAP_INDEX;
	if (list_index < ptr_list->pointer_list_size) {
		struct pointer_line *ptr_line = &ptr_list->pointer_list[list_index];

		INT_POINTER_TYPE i;
		for (i = 0; i < ptr_line->data_size; i ++) {
			if (ptr_line->data[i].id == ptr_id) {
				if (ptr_line->data[i].context != context)
					return (void *)0;
				return (void *)ptr_line->data[i].ptr;
			}
		}
	}

	return (void *)0;
}

void *pointerList_free(struct pointer_list *ptr_list, INT_POINTER_TYPE ptr_id, const void *context) {
	if ((ptr_id <= 0) || (!ptr_list))
		return (void *)0;

	INT_POINTER_TYPE list_index = (ptr_id - 1) % POINTER_WRAP_INDEX;
	if (list_index < ptr_list->pointer_list_size) {
		struct pointer_line *ptr_line = &ptr_list->pointer_list[list_index];

		INT_POINTER_TYPE i;
		for (i = 0; i < ptr_line->data_size; i ++) {
			if (ptr_line->data[i].id == ptr_id) {
				if (ptr_line->data[i].context != context)
					return (void *)0;

				ptr_line->data[i].id = 0;
				ptr_line->data[i].context = (void *)0;

				if (ptr_list->index == ptr_id) {
					ptr_list->index --;
				} else {
					INT_POINTER_TYPE j;
					int reuse_set = 0;
					for (j = ptr_list->reuse_ids_start; j < ptr_list->reuse_ids_size; j ++) {
						if (!ptr_list->reuse_ids[j]) {
							ptr_list->reuse_ids[j] = ptr_id;
							reuse_set = 1;
							ptr_list->reuse_ids_start = j + 1;
							break;
						} else
						if (j < ptr_list->reuse_ids_lookup_start)
							ptr_list->reuse_ids_lookup_start = j;

					}
					if (!reuse_set) {
						ptr_list->reuse_ids_lookup_start = 0;
						INT_POINTER_TYPE *old_ptr = ptr_list->reuse_ids;
						ptr_list->reuse_ids_size += POINTER_REUSE_INCREMENT;
						ptr_list->reuse_ids = (INT_POINTER_TYPE *)realloc(ptr_list->reuse_ids, ptr_list->reuse_ids_size * sizeof(INT_POINTER_TYPE));
						if (ptr_list->reuse_ids) {
							for (j = ptr_list->reuse_ids_size - POINTER_REUSE_INCREMENT + 1; j < ptr_list->reuse_ids_size; j ++)
								ptr_list->reuse_ids[j] = 0;
							ptr_list->reuse_ids[ptr_list->reuse_ids_size - POINTER_REUSE_INCREMENT] = ptr_id;
							ptr_list->reuse_ids_start = ptr_list->reuse_ids_size - POINTER_REUSE_INCREMENT + 1;
						} else {
							ptr_list->reuse_ids = old_ptr;
							ptr_list->reuse_ids_size -= POINTER_REUSE_INCREMENT;
						}
					}
				}

				return (void *)ptr_line->data[i].ptr;
			}
		}
	}

	return (void *)0;
}

void pointerList_deinit(struct pointer_list *ptr_list) {
	if (!ptr_list)
		return;

	INT_POINTER_TYPE i;
	for (i = 0; i < ptr_list->pointer_list_size; i ++) {
		if (ptr_list->pointer_list[i].data)
			free(ptr_list->pointer_list[i].data);
	}

	if (ptr_list->pointer_list)
		free(ptr_list->pointer_list);

	if (ptr_list->reuse_ids)
		free(ptr_list->reuse_ids);

	memset(ptr_list, 0, sizeof(struct pointer_list));
}

#endif
