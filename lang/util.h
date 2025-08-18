#ifndef _UTIL_H
#define _UTIL_H 1

#define WITH_CODE(c, message) \
    if((code = c)) { \
        printf(message, code); \
        return code; \
    } 

#endif
