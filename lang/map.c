#include <map.h>
#include <string.h>

int builtin_string_hash(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int builtin_string_comparator(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}
