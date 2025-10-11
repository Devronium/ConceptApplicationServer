#ifndef __POINTER_LIST_H
#define __POINTER_LIST_H

#include <stdlib.h>
#include <string.h>
#include <map>

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
	INT_POINTER_TYPE index;
};

void pointerList_init(struct pointer_list *ptr_list) {

	if (!ptr_list)
		return;

	ptr_list->index = 0;
}

INT_POINTER_TYPE pointerList_add(struct pointer_list *ptr_list, const void *ptr, const void *context) {

	if ((!ptr_list) || (!ptr))
		return 0;

	++ ptr_list->index;
	pointer_list_object ptr_2(ptr, context, ptr_list->index);
	ptr_list->map[ptr_list->index] = ptr_2;
	return ptr_list->index;
}

void *pointerList_pointer(struct pointer_list *ptr_list, INT_POINTER_TYPE ptr_id, const void *context) {

	if ((!ptr_id) || (!ptr_list))
		return (void *)0;

	pointer_list_object ptr_2 = ptr_list->map[ptr_id];
	if (ptr_2.context != context)
		return (void *)0;

	return (void *)ptr_2.ptr;
}

void *pointerList_free(struct pointer_list *ptr_list, INT_POINTER_TYPE ptr_id, const void *context) {
	if ((!ptr_id) || (!ptr_list))
		return (void *)0;

	pointer_list_object ptr_2 = ptr_list->map[ptr_id];
	if (ptr_2.context != context)
		return (void *)0;

	ptr_list->map.erase(ptr_id);

	if (ptr_id == ptr_list->index)
		ptr_list->index --;

	return (void *)ptr_2.ptr;
}

void pointerList_deinit(struct pointer_list *ptr_list) {
	if (!ptr_list)
		return;

	ptr_list->map.clear();
}

#endif
