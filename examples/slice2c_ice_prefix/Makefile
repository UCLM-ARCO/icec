# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR   = ../..
SLICE_DIR = /usr/share/slice
SLICE2C   = $(BASEDIR)/bin/slice2c

SLICE     = IceStorm.ice

all: $(SLICE:%.ice=%.h)

$(SLICE:%.ice=%.h): $(SLICE)
	$(RM) -rfv $@
	$(SLICE2C) --ice -I $(SLICE_DIR) $^

.PHONY: clean
clean:
	$(RM) -frv $(SLICE:%.ice=%.h)
	$(RM) -frv *.o
