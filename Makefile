# -*- mode: makefile-gmake; coding: utf-8 -*-

all:

.PHONY: test
test:
	-nosetests test/$(T) -s
	make -C examples clean > /dev/null

install:
	install -d $(DESTDIR)/usr/share/icec/bin
	install -m 755 bin/slice2c.py $(DESTDIR)/usr/share/icec/bin

	install -d $(DESTDIR)/usr/bin
	ln -s /usr/share/icec/bin/slice2c.py $(DESTDIR)/usr/bin/slice2c

	install -d $(DESTDIR)/usr/src/IceC
	install -m 644 IceC/*.c $(DESTDIR)/usr/src/IceC

	install -d $(DESTDIR)/usr/include/IceC
	install -m 644 IceC/*.h $(DESTDIR)/usr/include/IceC

arduino-pkg: clean
	DIR=$$(mktemp -d)/IceC; \
	mkdir -p $$DIR; \
	echo $$DIR; \
	cp -r arduino/* $$DIR; \
	mkdir -p $$DIR/src/IceC/platforms; \
	cp IceC/*.* $$DIR/src/IceC; \
	cp -r IceC/platforms/esp8266 $$DIR/src/IceC/platforms; \
	cp -r IceC/platforms/arduino $$DIR/src/IceC/platforms; \
	cd $$DIR/..; zip -r arduino-icec.zip IceC; \
	cd -; cp $$DIR/../arduino-icec.zip arduino-icec-$$(date +%Y%m%d).zip

.PHONY: clean
clean:
	make -C examples $@
	find -name "*.pyc" -delete
