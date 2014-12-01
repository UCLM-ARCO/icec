# -*- mode: makefile-gmake; coding: utf-8 -*-

TARGETS     = client server
SLICE       = sequences.ice
PROJECT_DIR = ../..

# CFLAGS  += -ggdb -DDEBUG -DDISSECT_MESSAGE
CFLAGS  += -I $(PROJECT_DIR)
CFLAGS  += -Wall -Wextra -pedantic -ansi
CFLAGS  += -Wno-unused-parameter -Wno-unused-function -Wno-variadic-macros
CFLAGS  += -DMAX_MESSAGE_SIZE=2048
LDLIBS  += -lm

vpath %.c $(PROJECT_DIR)/IceC

all: $(SLICE:%.ice=%.h) $(TARGETS)

$(SLICE:%.ice=%.h): $(SLICE)
	$(PROJECT_DIR)/bin/slice2c $^

$(TARGETS): IceC.o $@

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) $(SLICE:%.ice=%.h) *.o *~

