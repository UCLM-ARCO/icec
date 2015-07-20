// -*- mode: c++; coding: utf-8 -*-

//
// NOTE: this is a workaround for bug about these functions:
//  - https://github.com/esp8266/Arduino/issues/572
//
// These implementations are taken from:
//  - http://clc-wiki.net/wiki/C_standard_library
//

#include <string.h>
#include <strings.h>

size_t
strcspn(const char* s1, const char* s2) {
    size_t ret = 0;
    while (*s1)
        if (strchr(s2, *s1))
            return ret;
        else
            s1++, ret++;
    return ret;
}

size_t
strspn(const char* s1, const char* s2) {
    size_t ret = 0;
    while (*s1 && strchr(s2, *s1++))
        ret++;
    return ret;
}

// same implementation as strchr
char*
index(const char* s, int c) {
    while (*s != (char)c)
        if (!*s++)
            return 0;
    return (char*)s;
}
