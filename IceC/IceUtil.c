/* -*- mode: c; coding: utf-8 -*- */

#include <IceC/IceUtil.h>

void ICEC_FUNC_ATTR
IceUtil_getIdentity(const char* s, char* result) {
    int end;

    trace();
    s += strspn(s, "\n\r\t ");
    end = strcspn(s, "\n\r\t ");
    strncpy(result, s, end);
    result[end] = 0;
}

void ICEC_FUNC_ATTR
IceUtil_getProtocolType(const char* s, Ice_EndpointType* result) {
    char proto[MAX_PROTO_SIZE];
    const char* i;
    int end;

    trace();
    proto[0] = 0;

    s += strspn(s, "\n\r\t ");

    i = index(s, ':');
    if (i != NULL) {
        i++;
        s = i + strspn(i, "\n\r\t ");
    }

    end = strcspn(s, "\n\r\t ");
    strncpy(proto, s, end);
    proto[end] = 0;

    *result = 0;
    IcePlugin_getProtocolType(proto, result);

    if (*result == 0)
        assert(false && "Invalid endpoint type!");
}

void ICEC_FUNC_ATTR
IceUtil_getAdapterId(const char* s, char* result) {
    const char* i;
    int end;

    s += strspn(s, "\n\r\t ");

    i = index(s, '@');
    if (i != NULL) {
        i++;
        s = i + strspn(i, "\n\r\t ");

        end = strcspn(s, "\n\r\t ");
        strncpy(result, s, end);
        result[end] = 0;
    }
}

void ICEC_FUNC_ATTR
IceUtil_getMode(const char* s, Ice_ReferenceMode* mode) {
    ssize_t i, j;
    const char* t = " -";
    const char* sep;
    ssize_t token_size = 2;

    sep = strchr(s, ':');
    if (sep == NULL)
        sep = strchr(s, '@');

    int limit = sep - s;

    for (i=0; i<limit; i++) {
        for (j=0; j<token_size; j++) {
            if (s[i+j] != t[j])
                break;
            if ((j+1) == token_size && (i+j+1) < limit) {
                switch(s[i+j+1]) {
                case 'd': *mode = Ice_ReferenceModeDatagram; return;
                case 'o': *mode = Ice_ReferenceModeOneway; return;
                case 't': *mode = Ice_ReferenceModeTwoway; return;
                }
            }
        }
    }

    c_throw("Ice_InvalidEndpointException", __FILE__, __LINE__);
}

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
IceUtil_getByteParam(const char* s, byte* result, char flag) {
    char strvalue[10];  /* bigger for some extra security */
    IceUtil_getStringParam(s, strvalue, flag);
    *result = atoi(strvalue);
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

void ICEC_FUNC_ATTR
IceUtil_IdentityToString(IceStd_IdentityPtr oid, char* dst) {
    int i = 0;
    strncpy(dst, oid->category.value, oid->category.size);
    i += oid->category.size;
    dst[i] = '/';
    i++;
    strncpy(dst + i, oid->name.value, oid->name.size);
    i += oid->name.size;
    dst[i] = 0;
}
