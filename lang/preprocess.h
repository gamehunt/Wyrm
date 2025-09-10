#ifndef _PREPROCESS_H
#define _PREPROCESS_H

#include "map.h"

DEFINE_MAP_TYPE(compile_defs, const char*, int)

int preprocess(const char* in, char** out);
int preprocess_is_defined(compile_defs_map* local_defines, const char* key);

void preprocess_init(int count, const char** extra_defs);

#endif
