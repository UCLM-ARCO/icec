# -*- mode: makefile-gmake; coding: utf-8 -*-

all:

.PHONY: test
test:
	make -C examples clean all
	nosetests test/test_*.py -s

.PHONY: clean
clean:
	make -C examples $@
