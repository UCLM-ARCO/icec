# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC
SLICE2C  = $(BASEDIR)/bin/slice2c

TARGET     = publisher
SLICES     = example.ice IceStorm-lite.ice
IS_DB_HOME = /tmp/test-db

# CFLAGS  += -ggdb -DDEBUG -DICE_TRACE_PROTOCOL=1
CFLAGS  += -I $(BASEDIR)
CFLAGS  += -Wall -Wextra -pedantic -Wno-variadic-macros -Wno-unused-parameter \
	   -Wno-unused-function


vpath %.c $(ICEC_SRC)/
vpath %.c $(ICEC_SRC)/platforms/x86/


all: $(SLICES:%.ice=%.h) $(TARGET)

$(TARGET): IceC.o IceUtil.o TCPEndpoint.o port.o $@

%.h: %.ice
	$(RM) -rf $@
	$(SLICE2C) --ice -I/usr/share/slice $^

run-is:
	$(RM) -r $(IS_DB_HOME)
	mkdir -p $(IS_DB_HOME)
	icebox --Ice.Config=icestorm.config

.PHONY: clean
clean:
	$(RM) $(TARGET) $(SLICES:%.ice=%.h) *.o *~
	$(RM) -r $(IS_DB_HOME)
