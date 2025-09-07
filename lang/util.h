#ifndef _UTIL_H
#define _UTIL_H 1

#define WITH_CODE(c, message) \
    if((code = c)) { \
        printf(message, code); \
        return code; \
    } 

#define WITH_CODE_GOTO(c, message) \
    if((code = c)) { \
        printf(message, code); \
        goto error; \
    } 

#define SAFE_CALL(func, ...) \
	if(func) { \
		func(__VA_ARGS__); \
	}

#endif
