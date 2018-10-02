# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../../../..
SLICE2C  = $(BASEDIR)/bin/slice2c

SLICES   = IceObject.ice

all: $(SLICES:%.ice=%.h)

%.h: %.ice
	$(RM) -rf $@
	$(SLICE2C) --ice --underscore -I/usr/share/slice $^


.PHONY: clean
clean:
	$(RM) $(SLICES:%.ice=%.h) *.o *~

