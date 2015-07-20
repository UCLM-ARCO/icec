# -*- mode: makefile-gmake; coding: utf-8 -*-

SLICE = led_iface.ice

all:
	slice2c $(SLICE)

.PHONY: clean
clean:
	$(RM) $(SLICE:%.ice=%.h)
