#ifndef _MAP_H
#define _MAP_H

#define MAP_SIZE 64

#define DEFINE_MAP_TYPE(value_type, key_type, name, hash_function, key_comparator) \
    typedef struct _##name##_val_wrapper { \
        key_type key; \
        value_type value; \
        struct _##name##_val_wrapper* next; \
    } name##_val_wrapper; \
    typedef struct { \
        name##_val_wrapper* data[MAP_SIZE]; \
    } name##_map; \
    static int name##_hash(key_type key) { \
        return hash_function(key) % MAP_SIZE; \
    } \
    static name##_map* name##_map_create() { \
        name##_map* m = calloc(1, sizeof(name##_map)); \
        return m; \
    } \
    static void _##name##_wrapper_free(name##_val_wrapper* wrapper) { \
        while(wrapper) { \
            name##_val_wrapper* next = wrapper->next; \
            free(wrapper); \
            wrapper = next; \
        } \
    } \
    static void name##_map_free(name##_map* map) { \
        for(int i = 0; i < MAP_SIZE; i++) { \
            _##name##_wrapper_free(map->data[i]); \
        } \
        free(map); \
    } \
    static value_type* name##_map_get(name##_map* map, key_type key) { \
        int hash = name##_hash(key); \
        name##_val_wrapper* wrapper = map->data[hash]; \
        if (wrapper) { \
            if(wrapper->next == NULL) { \
                return &wrapper->value; \
            } else { \
                while(wrapper && key_comparator(wrapper->key, key) == 0) { \
                    wrapper = wrapper->next; \
                } \
                if(wrapper) { \
                    return &wrapper->value; \
                } else { \
                    return NULL; \
                } \
            } \
        } else { \
            return NULL; \
        } \
    } \
    static int name##_map_contains(name##_map* map, key_type key) { \
        return name##_map_get(map, key) != NULL; \
    } \
    static name##_val_wrapper* name##_create_wrapper(key_type key, value_type value) { \
        name##_val_wrapper* wrapper = calloc(1, sizeof(name##_val_wrapper)); \
        wrapper->key = key; \
        wrapper->value = value; \
        wrapper->next = NULL; \
        return wrapper; \
    } \
    static int name##_map_insert(name##_map* map, key_type key, value_type value) { \
        int hash = name##_hash(key); \
        name##_val_wrapper* wrapper = map->data[hash];\
        if(wrapper == NULL) { \
            wrapper = name##_create_wrapper(key, value); \
        } else { \
            name##_val_wrapper* parent = NULL; \
            while(wrapper && key_comparator(wrapper->key, key) == 0) { \
                parent = wrapper; \
                wrapper = wrapper->next; \
            } \
            if(wrapper) { \
                wrapper->value = value; \
            } else { \
                wrapper = name##_create_wrapper(key, value); \
                if(parent) { \
                    parent->next = wrapper; \
                } \
            } \
        } \
        map->data[hash] = wrapper; \
        return 0; \
    } 

int builtin_string_hash(const char* str);
int builtin_string_comparator(const char* a, const char* b);

#endif
