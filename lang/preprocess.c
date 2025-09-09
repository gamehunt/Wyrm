#include "preprocess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    	// Copy parts
    	strncpy(new_result, result, foundPos - result); // Part before oldSub
    	new_result[foundPos - result] = '\0'; // Null-terminate for strcat
    	strcat(new_result, newSub); // Add newSub
    	strcat(new_result, foundPos + oldSubLen); // Add part after oldSub

		result = new_result;
		originalLen = strlen(result);
	}
}

void expand_macro(const char* name, const char* value, size_t i, char** out) {
	char* part = _replace_substring(&(*out)[i], name, value);
	char* res  = malloc(strlen(part) + i);

	strncpy(res, *out, i);
	strncpy(res + i, part, strlen(part));

	free(part);
	free(*out);

	*out = res;
}

int directive(char** out, size_t* i) {
	char* copy = strdup(*out);
	char* directive = strtok(&copy[*i], " ");	

	*i += strlen(directive);

	char* args = strtok(NULL, "\n");

	*i += strlen(args);

	if(!strcmp(directive, "define")) {
		char* name  = strtok(args, " ");
		char* value = strtok(NULL, "\n");
		expand_macro(name, value, *i, out);
	}

	free(copy);

	return 0;
}

int preprocess(const char* in, char** _out) {
	char* out = strdup(in);

	for(size_t i = 0; i <= strlen(out); i++) {
		if(out[i] == '#') {
			i++;
			directive(&out, &i);
		}
	}

	*_out = out;
	return 0;
}
