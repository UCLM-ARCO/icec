# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ..
SLICE2C  = $(BASEDIR)/bin/slice2c
SLICES   = Locator.ice

vpath %.ice /usr/share/slice/Ice/


all: $(SLICES:%.ice=%.h)

Locator.h: Locator.ice
	$(RM) -rf $@
	$(SLICE2C) --ice --iface Ice_Locator -I/usr/share/slice $^

%.h: %.ice
	$(RM) -rf $@
	$(SLICE2C) --ice -I/usr/share/slice $^

.PHONY: clean
clean:
	$(RM) $(SLICES:%.ice=%.h) *.o *~
