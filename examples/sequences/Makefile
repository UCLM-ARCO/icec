# -*- mode: makefile-gmake; coding: utf-8 -*-

TARGETS     = client server
PROJECT_DIR = ../..

# CFLAGS  += -ggdb -DDEBUG -DDISSECT_MESSAGE
CFLAGS  += -I $(PROJECT_DIR)
CFLAGS  += -Wall -Wextra -pedantic -ansi
CFLAGS  += -Wno-unused-parameter -Wno-unused-function -Wno-variadic-macros
CFLAGS  += -DMAX_MESSAGE_SIZE=2048
LDLIBS  += -lm

vpath %.c $(PROJECT_DIR)/IceC

all: slice.h $(TARGETS)

slice.h: slice.ice
	$(PROJECT_DIR)/bin/slice2c $^

$(TARGETS): IceC.o $@

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) slice.h *.o *~

