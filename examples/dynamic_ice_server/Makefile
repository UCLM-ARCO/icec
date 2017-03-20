# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC

TARGETS ?= server
# CFLAGS  += -ggdb -DDEBUG -DICE_TRACE_PROTOCOL=1
CFLAGS  += -I $(BASEDIR)
CFLAGS  += -Wall -Wextra -pedantic -Wno-variadic-macros -Wno-unused-parameter \
	   -Wno-unused-function

vpath %.c $(ICEC_SRC)
vpath %.c $(ICEC_SRC)/platforms/x86/

all: $(TARGETS)

$(TARGETS): IceC.o IceUtil.o TCPEndpoint.o port.o $@

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS)
	$(RM) -frv *.o
