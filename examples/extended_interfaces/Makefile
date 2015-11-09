# -*- mode: makefile-gmake; coding: utf-8 -*-

TARGETS     = client server
SLICE       = slice.ice
PROJECT_DIR = ../..

# CFLAGS  += -ggdb -DDEBUG -DICE_TRACE_PROTOCOL
CFLAGS  += -I $(PROJECT_DIR)
CFLAGS  += -Wall -Wextra -pedantic -ansi
CFLAGS  += -Wno-unused-parameter -Wno-unused-function -Wno-variadic-macros
LDLIBS  += -lm

vpath %.c $(PROJECT_DIR)/IceC
vpath %.c $(PROJECT_DIR)/IceC/platforms/x86/

all: $(SLICE:%.ice=%.h) $(TARGETS)

$(SLICE:%.ice=%.h): $(SLICE)
	$(PROJECT_DIR)/bin/slice2c $^

$(TARGETS): IceC.o IceUtil.o UDPEndpoint.o port.o $@

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) $(SLICE:%.ice=%.h) *.o *~
