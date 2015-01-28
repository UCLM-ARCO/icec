# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC
SLICE2C  = $(BASEDIR)/bin/slice2c

TARGETS ?= writer_server
SLICE    = hello.ice
# CFLAGS  += -ggdb -DDEBUG
CFLAGS  += -I $(BASEDIR)
CFLAGS  += -Wall -Wextra -pedantic -Wno-variadic-macros -Wno-unused-parameter \
	   -Wno-unused-function

vpath %.c $(ICEC_SRC)
vpath %.c $(ICEC_SRC)/platforms/x86/

all: $(SLICE:%.ice=%.h) $(TARGETS)

$(TARGETS): IceC.o IceUtil.o UDPEndpoint.o port.o $@

$(SLICE:%.ice=%.h): $(SLICE)
	$(RM) -rfv $@
	$(SLICE2C) $^

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) $(SLICE:%.ice=%.h)
	$(RM) -frv *.o
