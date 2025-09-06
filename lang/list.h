#ifndef _LIST_H
#define _LIST_H

#include <stdlib.h>

#define DEFINE_LIST_TYPE(name, el_type) \
typedef struct _##name##_list { \
	int capacity; \
	int size; \
	el_type* data; \
} name##_list; \
name##_list* name##_list_create() { \
	name##_list* l = calloc(1, sizeof(name##_list));\
	l->capacity = 4; \
	l->data = calloc(4, sizeof(el_type)); \
	return l; \
}\
void name##_list_free(name##_list* l) { \
	free(l); \
} \
void name##_list_append(name##_list* l, el_type el) { \
	if(l->size == l->capacity) { \
		l->capacity *= 2; \
		l->data = reallocarray(l->data, l->capacity, sizeof(el_type)); \
	} \
	l->data[l->size] = el; \
	l->size++; \
}

#endif
