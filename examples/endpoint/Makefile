# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC
SLICE2C  = $(BASEDIR)/bin/slice2c

TARGETS ?= server client
SLICE    = slice.ice
CFLAGS  += -I $(BASEDIR)
CFLAGS  += -DDEBUG -ggdb

vpath %.c $(ICEC_SRC)


all: $(SLICE:%.ice=%.h) $(TARGETS)

$(TARGETS): IceC.o myplugin.o $@

$(SLICE:%.ice=%.h): $(SLICE)
	$(RM) -rfv $@
	$(SLICE2C) $^

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) $(SLICE:%.ice=%.h)
	$(RM) -rfv *.o
