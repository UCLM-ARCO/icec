/* -*- mode: c; coding: utf-8; truncate-lines: true -*- */

#include <IceC/IceC.h>
#include <sys/select.h>

void
IceInternal_selectOnAdapters(Ice_ObjectAdapter_ListPtr adapters,
			     Ice_ObjectAdapterPtr result[]) {

    int i, count;
    fd_set read_set;
    Ice_ObjectAdapterPtr adapter;

    int fds[MAX_NUM_OF_ADAPTERS];
    Ice_ObjectAdapterPtr map[MAX_NUM_OF_ADAPTERS];

    trace();
    FD_ZERO(&read_set);

    count = 0;
    for (i=0; i<adapters->count; i++) {
	adapter = *(adapters->begin + i);
	if (! adapter->activated) {
	    continue;
	}

	FD_SET(adapter->connection.fd, &read_set);
	fds[count] = adapter->connection.fd;
	map[count] = adapter;
	count++;
    }

    if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
	error("Error while reading from descriptors: %s\n", strerror(errno));
	return;
    }

    for (i=0; i<MAX_NUM_OF_ADAPTERS; i++)
	result[i] = NULL;

    for (i=0; i<count; i++) {
	if (! FD_ISSET(fds[i], &read_set)) {
	    continue;
	}

	result[i] = map[i];
    }
}
