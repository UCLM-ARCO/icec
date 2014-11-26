# -*- mode: makefile-gmake; coding: utf-8 -*-

all:

.PHONY: test
test:
	make -C examples clean all
	-nosetests test/test_*.py -s

install:
	install -d $(DESTDIR)/usr/share/icec/bin
	install -m 755 bin/slice2c.py $(DESTDIR)/usr/share/icec/bin

	install -d $(DESTDIR)/usr/bin
	ln -s /usr/share/icec/bin/slice2c.py $(DESTDIR)/usr/bin/slice2c

	install -d $(DESTDIR)/usr/src/IceC
	install -m 644 IceC/*.c $(DESTDIR)/usr/src/IceC

	install -d $(DESTDIR)/usr/share/include/IceC
	install -m 644 IceC/*.h $(DESTDIR)/usr/share/include/IceC

.PHONY: clean
clean:
	make -C examples $@
