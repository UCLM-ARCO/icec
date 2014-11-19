/* -*- mode: c; coding: utf-8 -*- */

#ifndef _ICEC_DEBUG_H_
#define _ICEC_DEBUG_H_

#define RED "\033[01;31m"
#define CLR "\033[00m"

#ifndef DEBUG

#define fixme(...)
#define trace()
#define error(...)
#define hexdump(...)

#else

#define fixme(MSG) printf(RED "FIXME:" CLR " %s (%s:%d %s)\n", \
			  MSG, __FILE__, __LINE__, __func__)
#define trace() printf(" -> %s:%d %s\n", __FILE__, __LINE__, __func__)
#define error(...) printf(RED "ERROR:" CLR); printf(__VA_ARGS__);

#define byte unsigned char
#define Ice_Short int16_t
#define Ice_Int int32_t

static char buf[128];

// from: http://stackoverflow.com/a/7776146
static void
hexdump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    if (desc != NULL)
        printf ("%s:\n", desc);

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                printf ("  %s\n", buff);
            printf ("  %04x ", i);
        }

        printf (" %02x", pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    printf ("  %s\n", buff);
}

static byte*
dissect_string(byte* data, const char* name) {

    byte size = *(data++);
    printf(" - 1 %s.size: %d bytes\n", name, size);

    // bigger strings not supported
    assert(size != 255);

    strncpy(buf, data, size); buf[size] = 0;
    printf(" - %d %s: '%s'\n", size, name, buf);
    return data + size;;
}

static byte*
dissect_identity(byte* data) {
    data = dissect_string(data, "id.name");
    data = dissect_string(data, "id.category");
    return data;
}

static byte*
dissect_none(byte* data, const char* name) {

    printf(" - 1 %s: %d\n", name, *data);
    assert(*data == 0);
    return data + 1;
}

static byte*
dissect_byte(byte* data, const char* name) {
    printf(" - 1 %s: %d\n", name, *data);
    return data + 1;
}

static byte*
dissect_byte_eq(byte* data, const char* name, byte expected) {
    printf(" - 1 %s: %d\n", name, *data);
    assert(*data == expected);
    return data + 1;
}

static byte*
dissect_byte_lt(byte* data, const char* name, byte limit) {
    printf(" - 1 %s: %d\n", name, *data);
    assert(*data < limit);
    return data + 1;
}

static byte*
dissect_short_eq(byte* data, const char* name, Ice_Short expected) {

    Ice_Short* value = (Ice_Short*)data;
    printf(" - 2 %s: %d\n", name, *value);
    assert(*value == expected);
    return data + 2;
}

static byte*
dissect_int(byte* data, const char* name) {

    Ice_Int* value = (Ice_Int*)data;
    printf(" - 4 %s: %d\n", name, *value);
    return data + 4;
}

static byte*
dissect_int_eq(byte* data, const char* name, Ice_Int expected) {

    Ice_Int* value = (Ice_Int*)data;
    printf(" - 4 %s: %d\n", name, *value);
    assert(*value == expected);
    return data + 4;
}

static byte*
dissect_encaps(byte* data, const char* name) {
    int size = *((Ice_Int*)data);
    printf(" - 4 %s.size: %d\n", name, size);
    data += 4;

    printf(" - 1 %s.major: %d\n", name, *(data++));
    printf(" - 1 %s.minor: %d\n", name, *(data++));
    return data;
}

static byte*
dissect_byteseq(byte* data, const char* name) {
    byte size = *data;
    printf("   - 1 %s.size: %d\n", name, size);
    assert(size < 255); // no bigger sequences supported
    data++;

    // FIXME: try to print data

    return data + size;
}

static byte*
dissect_udp_proxy(byte* data, int limit) {

    hexdump("\nUDP Proxy", data, limit);
    printf("\nUDP Proxy dissection:\n");

    data = dissect_identity(data);
    data = dissect_none(data, "facetSeq.size");
    data = dissect_byte(data, "mode");
    data = dissect_byte(data, "secure");
    data = dissect_byte_eq(data, "endpointSeq.size", 1);
    data = dissect_short_eq(data, "endpoint.type", 3);
    data = dissect_encaps(data, "encapsulation");
    data = dissect_string(data, "host");
    data = dissect_int(data, "port");
    data = dissect_byte(data, "protocolMajor");
    data = dissect_byte(data, "protocolMinor");
    data = dissect_byte(data, "encodingMajor");
    data = dissect_byte(data, "encodingMinor");
    data = dissect_byte_eq(data, "compress", 0);

    return data;
}

static void
dissect_AutoPilot_ParamSeq(byte* data, int limit) {
    hexdump("\nAutoPilot ParamSeq", data, limit);
    printf("\nParamSeq dissection:\n");

    byte size = *data;
    data = dissect_byte_lt(data, "paramSeq.size", 255);

    int i;
    for (i=0; i<size; i++) {
	printf(" ------- param %d\n", i);

	data = dissect_byte(data, "id");
	data = dissect_byte(data, "type");
	data = dissect_byteseq(data, "data");
    }
}

static void
dissect_message(byte* data, int limit) {
    hexdump("Ice Message", data, limit);
    printf("\nIce Message dissection:\n");

    data = dissect_byte_eq(data, "magic", 'I');
    data = dissect_byte_eq(data, "magic", 'c');
    data = dissect_byte_eq(data, "magic", 'e');
    data = dissect_byte_eq(data, "magic", 'P');
    data = dissect_byte(data, "protocolMajor");
    data = dissect_byte(data, "protocolMinor");
    data = dissect_byte(data, "encodingMajor");
    data = dissect_byte(data, "encodingMinor");

    data = dissect_byte_eq(data, "messageType", 0);
    data = dissect_byte_eq(data, "compressionStatus", 0);
    data = dissect_int_eq(data, "messageSize", limit);
    data = dissect_int(data, "requestId");
    data = dissect_identity(data);
    data = dissect_byte_eq(data, "facetSeq.size", 0);
    data = dissect_string(data, "operation");
    data = dissect_byte(data, "mode");
    data = dissect_byte_eq(data, "context.size", 0);

    int size = *((Ice_Int*)data);
    data = dissect_encaps(data, "encapsulation");
    byte* rest = dissect_udp_proxy(data, size - 6);
    dissect_AutoPilot_ParamSeq(rest, (size - 6) - (rest - data));
}

#undef byte
#undef Ice_Short
#undef Ice_Int

#endif  // DEBUG
#endif  // _ICEC_DEBUG_H_
