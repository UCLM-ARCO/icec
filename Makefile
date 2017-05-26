# -*- mode: makefile-gmake; coding: utf-8 -*-

TEST_RUNNER ?= prego

all:

.PHONY: tests
tests:
	$(TEST_RUNNER) test/$(T)
	make -C examples clean > /dev/null

install: ARDUINO_DIST = $(DESTDIR)/usr/share/icec/arduino
install:
	install -d $(DESTDIR)/usr/share/icec/bin
	install -m 755 bin/slice2c.py $(DESTDIR)/usr/share/icec/bin

	install -d $(DESTDIR)/usr/bin
	ln -s /usr/share/icec/bin/slice2c.py $(DESTDIR)/usr/bin/slice2c

	install -d $(DESTDIR)/usr/src/IceC
	install -m 644 IceC/*.c $(DESTDIR)/usr/src/IceC

	install -d $(DESTDIR)/usr/include/IceC
	install -m 644 IceC/*.h $(DESTDIR)/usr/include/IceC

####### arduino dist
	install -d $(ARDUINO_DIST)
	cp -r arduino/* $(ARDUINO_DIST)

	install -d $(ARDUINO_DIST)/src/IceC
	install -m 644 IceC/IceC.h $(ARDUINO_DIST)/src
	install -m 644 IceC/*.* $(ARDUINO_DIST)/src/IceC

	install -d $(ARDUINO_DIST)/src/IceC/platforms
	cp -r IceC/platforms/esp8266 $(ARDUINO_DIST)/src/IceC/platforms
	cp -r IceC/platforms/arduino $(ARDUINO_DIST)/src/IceC/platforms

arduino-package: ARDUINO_DIST=/tmp/icec/arduino/IceC
arduino-package: clean
	$(RM) -rf $(ARDUINO_DIST)

	install -d $(ARDUINO_DIST)
	cp -r arduino/* $(ARDUINO_DIST)

	install -d $(ARDUINO_DIST)/src/IceC
	install -m 644 IceC/IceC.h $(ARDUINO_DIST)/src
	install -m 644 IceC/*.* $(ARDUINO_DIST)/src/IceC

	install -d $(ARDUINO_DIST)/src/IceC/platforms
	cp -r IceC/platforms/esp8266 $(ARDUINO_DIST)/src/IceC/platforms
	cp -r IceC/platforms/arduino $(ARDUINO_DIST)/src/IceC/platforms

	cd $(ARDUINO_DIST)/..; zip -r arduino-icec-$$(date +%Y%m%d).zip IceC
	mv $(ARDUINO_DIST)/../*.zip .


.PHONY: clean
clean:
	-make -C examples $@
	find -name "*.pyc" -delete
	$(RM) *.zip
