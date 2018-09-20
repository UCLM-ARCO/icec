# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC
SLICE2C  = $(BASEDIR)/bin/slice2c

TARGET   = client
SLICES   = example.ice

CFLAGS  += -I $(BASEDIR)
# CFLAGS  += -ggdb -DDEBUG -DICE_TRACE_PROTOCOL=1
# CFLAGS  += -Wall -Wextra -pedantic -Wno-variadic-macros -Wno-unused-parameter \
# 	   -Wno-unused-function


vpath %.c $(ICEC_SRC)/
vpath %.c $(ICEC_SRC)/platforms/x86/


all: $(SLICES:%.ice=%.h) $(TARGET)

$(TARGET): $(SLICES:%.ice=%.h) IceC.o IceUtil.o TCPEndpoint.o port.o $@

%.h: %.ice
	$(RM) -rf $@
	$(SLICE2C) --ice -I/usr/share/slice $^


.PHONY: clean
clean:
	$(RM) $(TARGET) $(SLICES:%.ice=%.h) *.o *~
