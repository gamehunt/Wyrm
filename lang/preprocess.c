#include "preprocess.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MAP_IMPL(compile_defs, const char*, int, builtin_string_hash, builtin_string_comparator)

static compile_defs_map* _global_compile_defs = NULL;

int preprocess_is_defined(compile_defs_map* local_defines, const char* key) {
	return compile_defs_map_contains(_global_compile_defs, key) ||
		   compile_defs_map_contains(local_defines, key);
}

static char* _replace_substring(const char* original, const char* oldSub, const char* newSub) {
    char* foundPos;
    int oldSubLen = strlen(oldSub);
    int newSubLen = strlen(newSub);
    int originalLen = strlen(original);

	char* result = strdup(original);

	while(1) {
    	foundPos = strstr(result, oldSub);

    	if (foundPos == NULL) {
    	    return result;
    	}

    	int newLen = originalLen - oldSubLen + newSubLen;
    	char* new_result = (char*)malloc(newLen + 1);
    	if (new_result == NULL) return NULL;

    	strncpy(new_result, result, foundPos - result);
    	new_result[foundPos - result] = '\0';
    	strcat(new_result, newSub);
    	strcat(new_result, foundPos + oldSubLen);

		result = new_result;
		originalLen = strlen(result);
	}
}

void expand_macro(const char* name, const char* value, size_t i, char** out) {
	if(value == NULL) {
		value = "";
	}

	char* part = _replace_substring(&(*out)[i], name, value);
	char* res  = malloc(strlen(part) + i);

	strncpy(res, *out, i);
	strncpy(res + i, part, strlen(part));

	free(part);
	free(*out);

	*out = res;
}

void _remove(int from, int to, char* out) {
	for(;from < to;from++) {
		out[from] = ' ';
	}	
}

void conditional(char** out, size_t i, int success, char* branch, char* end) {
	if(success) {
		if(branch) {
			_remove(branch - *out + 6, end - *out - 1, *out);
		}
	} else {
		if(branch) {
			_remove(i + 2, branch - *out - 1, *out);
		} else {
			_remove(i + 2, end - *out - 1, *out);
		}
	}
}

int directive(char** out, size_t* i, compile_defs_map* m) {
	char* copy = strdup(*out);
	char* directive = strtok(&copy[*i], " ");	

	*i += strlen(directive);

	char* args = strtok(NULL, "\n");

	*i += strlen(args);

	if(!strcmp(directive, "define")) {
		char* name  = strtok(args, " ");
		char* value = strtok(NULL, "\n");
		compile_defs_map_insert(m, name, 1);
		expand_macro(name, value, *i, out);
	} else if (!strcmp(directive, "ifdef")) {
		char* name   = strtok(args, "\n");
		char* end    = strstr(*out, "#endif");
		char* branch = strstr(*out, "#else");
		conditional(out, *i, preprocess_is_defined(m, name), branch, end);
	} else if (!strcmp(directive, "ifndef")) {
		char* name   = strtok(args, "\n");
		char* end    = strstr(*out, "#endif");
		char* branch = strstr(*out, "#else");
		conditional(out, *i, !preprocess_is_defined(m, name), branch, end);
	} else if (strcmp(directive, "endif")) {
		printf("Preprocessor warning: unknown directive: %s\n", directive);
	}

	free(copy);

	return 0;
}

int preprocess(const char* in, char** _out) {
	char* out = strdup(in);

	compile_defs_map* defs = compile_defs_map_create();

	for(size_t i = 0; i <= strlen(out); i++) {
		if(out[i] == '#') {
			i++;
			int r;
			if((r = directive(&out, &i, defs))) {
				return r;
			}
		}
	}

	*_out = out;
	return 0;
}

void preprocess_init(int count, const char** extra) {
	_global_compile_defs = compile_defs_map_create();
	for(int i = 0; i < count; i++) {
		compile_defs_map_insert(_global_compile_defs, extra[i], 1);
	}
}
