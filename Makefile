# -*- mode: makefile-gmake; coding: utf-8 -*-

all:

.PHONY: test
test:
	make -C examples/hello
	nosetests test/test_*.py -s

.PHONY: clean
clean:
	make -C examples/hello $@
