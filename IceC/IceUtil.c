/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceUtil.h>

void ICEC_FUNC_ATTR
IceUtil_getHost(const char* s, char* result) {
    IceUtil_getStringParam(s, result, 'h');
}

void ICEC_FUNC_ATTR
IceUtil_getPort(const char* s, uint16_t* result) {
    char strport[10];
    IceUtil_getStringParam(s, strport, 'p');
    *result = atoi(strport);
}

void ICEC_FUNC_ATTR
IceUtil_getStringParam(const char* s, char* result, char flag) {
    int end;

    trace();
    result[0] = 0;

    while (true) {
	s = index(s, '-');
	if (s == NULL) return;
	if (s[1] == flag) {
	    s += 2;  /* '-' + flag */
	    s += strspn(s, "\n\r\t ");
	    end = strcspn(s, "\n\r\t ");
	    strncpy(result, s, end);
	    result[end] = 0;
	    return;
	}
	s++;
    }
}
