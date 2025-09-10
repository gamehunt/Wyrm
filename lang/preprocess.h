#ifndef _PREPROCESS_H
#define _PREPROCESS_H

#include "map.h"

DEFINE_MAP_TYPE(compile_defs, const char*, int)

enum directives {
	D_INCLUDE,
	D_DEFINE,
	D_IFDEF,
	D_IFNDEF,
	D_ELSE,
	D_ENDIF,
	D_ERROR,
	D_WARNING,
	D_LINE
};

int preprocess(const char* in, char** out);
int preprocess_is_defined(compile_defs_map* local_defines, const char* key);
enum directives* preprocess_get_directive(const char* key);

void preprocess_init(int count, const char** extra_defs);

#endif
