# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC
SLICE2C  = $(BASEDIR)/bin/slice2c

TARGETS ?= reader_server writer_client
SLICE    = slice.ice
CFLAGS  += -I $(BASEDIR)

vpath %.c $(ICEC_SRC)


all: $(SLICE:%.ice=%.h) $(TARGETS)

$(TARGETS): IceC.o $@

$(SLICE:%.ice=%.h): $(SLICE)
	$(RM) -rfv $@
	$(SLICE2C) $^

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) $(SLICE:%.ice=%.h)
