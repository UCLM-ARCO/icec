# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC

TARGET   = test

# CFLAGS  += -ggdb -DDEBUG -DICE_TRACE_PROTOCOL=1
# CFLAGS  += -Wall -Wextra -pedantic -Wno-variadic-macros -Wno-unused-parameter \
# 	   -Wno-unused-function
CFLAGS  += -I $(BASEDIR)

vpath %.c $(ICEC_SRC)/
vpath %.c $(ICEC_SRC)/platforms/x86/


all: $(TARGET)

$(TARGET): IceC.o IceUtil.o TCPEndpoint.o port.o $@


.PHONY: clean
clean:
	$(RM) $(TARGET) *.o *~
