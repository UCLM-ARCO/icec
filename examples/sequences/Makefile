# -*- mode: makefile-gmake; coding: utf-8 -*-

TARGETS     = client server
PROJECT_DIR = ../..

# CFLAGS  += -ggdb -DDEBUG
CFLAGS  += -Wall -Wextra -pedantic -ansi
CFLAGS  += -Wno-unused-parameter -Wno-unused-function

vpath %.c $(PROJECT_DIR)/IceC

# all: slice.h $(TARGETS)
all: $(TARGETS)

slice.h: slice.ice
	$(PROJECT_DIR)/bin/slice2c $^

$(TARGETS): IceC.o $@

.PHONY: clean
clean:
#	$(RM) -frv $(TARGETS) slice.h *.o *~
	$(RM) -frv $(TARGETS) *.o *~

