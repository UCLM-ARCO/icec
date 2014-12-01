# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../..
ICEC_SRC = $(BASEDIR)/IceC
SLICE2C  = $(BASEDIR)/bin/slice2c

TARGETS ?= server client
SLICE    = example.ice
CFLAGS  += -I $(BASEDIR)
CFLAGS  += -DDEBUG -ggdb

# Uncomment for very strict c code
CFLAGS  += -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes \
	   -Wstrict-prototypes -Wold-style-definition -O3

# Add a bit of sense after that...
CFLAGS  += -Wno-variadic-macros -Wno-sign-compare -Wno-implicit-function-declaration \
	   -Wno-unused-parameter -Wno-unused-function

vpath %.c $(ICEC_SRC)


all: $(SLICE:%.ice=%.h) $(TARGETS)

$(TARGETS): IceC.o myplugin.o $@

$(SLICE:%.ice=%.h): $(SLICE)
	$(RM) -rfv $@
	$(SLICE2C) $^

.PHONY: clean
clean:
	$(RM) -frv $(TARGETS) $(SLICE:%.ice=%.h)
	$(RM) -rfv *.o
