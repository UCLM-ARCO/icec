# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC
SLICE2C  = $(BASEDIR)/bin/slice2c

TARGET   = publisher
SLICES   = example.ice IceStorm-lite.ice
REG_HOME = /tmp/test-registry
NOD_HOME = /tmp/test-node

CFLAGS  += -ggdb -DDEBUG -DICE_TRACE_PROTOCOL=1
CFLAGS  += -I $(BASEDIR)
CFLAGS  += -Wall -Wextra -pedantic -Wno-variadic-macros -Wno-unused-parameter \
	   -Wno-unused-function


vpath %.ice /usr/share/slice/IceStorm/
vpath %.c $(ICEC_SRC)/
vpath %.c $(ICEC_SRC)/platforms/x86/


all: $(SLICES:%.ice=%.h) $(TARGET)

$(TARGET): $(SLICES:%.ice=%.h) IceC.o IceUtil.o TCPEndpoint.o port.o $@

%.h: %.ice
	$(RM) -rf $@
	$(SLICE2C) --ice -I/usr/share/slice $^

run-icegrid: DETACH ?= gnome-terminal --
run-icegrid: run-registry run-node load-application

run-registry:
	$(RM) -r $(REG_HOME)
	mkdir -p $(REG_HOME)
	$(DETACH) icegridregistry --Ice.Config=registry.config

run-node:
	$(RM) -r $(NOD_HOME)
	mkdir -p $(NOD_HOME)/node1
	$(DETACH) icegridnode --Ice.Config=node.config

load-application:
	sleep 2; \
	icegridadmin -u user -p pass -H 127.0.0.1 -e "application add application.xml"


.PHONY: clean
clean:
	$(RM) $(TARGET) $(SLICES:%.ice=%.h) *.o *~
	$(RM) -r $(REG_HOME) $(NOD_HOME)
