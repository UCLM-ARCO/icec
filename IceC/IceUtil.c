/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceUtil.h>

#include <string.h>
#include <strings.h>

void
IceUtil_getHost(const char* s, char* result) {
    int end;

    trace();
    result[0] = 0;

    while (true) {
	s = index(s, '-');
	if (s == NULL) return;
	if (s[1] == 'h') {
	    s += 2;  /* '-h' */
	    s += strspn(s, "\n\r\t ");
	    end = strcspn(s, "\n\r\t ");
	    strncpy(result, s, end);
	    result[end] = 0;
	    return;
	}
	s++;
    }
}

void
IceUtil_getPort(const char* s, uint16_t* result) {
    char strport[10];
    int end;

    trace();
    *result = -1;

    while (true) {
	s = index(s, '-');
	if (s == NULL) return;
	if (s[1] == 'p') {
	    s += 2; /* '-p' */
	    s += strspn(s, "\n\r\t ");
	    end = strcspn(s, "\n\r\t ");
	    strncpy(strport, s, end);
	    strport[end] = 0;

	    *result = atoi(strport);
	    return;
	}
	s++;
    }
}
