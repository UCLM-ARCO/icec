# -*- mode: makefile-gmake; coding: utf-8 -*-

TARGETS     = client server
SLICE       = floats.ice
PROJECT_DIR = ../..

# CFLAGS  += -ggdb -DDEBUG -DICE_TRACE_PROTOCOL
CFLAGS  += -I $(PROJECT_DIR) \
	   -DMAX_MESSAGE_SIZE=2048 \
	   -Wall -Wextra -pedantic -ansi \
	   -Wno-unused-parameter \
	   -Wno-unused-function \
	   -Wno-variadic-macros \
	   -Wno-declaration-after-statement

LDLIBS  += -lm

vpath %.c $(PROJECT_DIR)/IceC
vpath %.c $(PROJECT_DIR)/IceC/platforms/x86/

all: $(SLICE:%.ice=%.h) $(TARGETS)

$(SLICE:%.ice=%.h): $(SLICE)
	$(PROJECT_DIR)/bin/slice2c $^

$(TARGETS): IceC.o IceUtil.o TCPEndpoint.o port.o $@

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) $(SLICE:%.ice=%.h) *.o *~
