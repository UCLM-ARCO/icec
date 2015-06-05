# -*- mode: makefile-gmake; coding: utf-8 -*-

CONTIKI              = /usr/share/contiki
TARGET               = z1
APP                  = client
WITH_UIP6            = 1
UIP_CONF_IPV6        = 1

SLICE                = hello.ice

PROJECT_DIR          = ../../..
PROJECT_SOURCEFILES += IceC.c IceUtil.c port.c UDPEndpoint.c
CONTIKI_SOURCEFILES += assert.c

# CFLAGS              += -DDEBUG
CFLAGS              += -Wno-unused-function -Wno-unused-variable
CFLAGS              += -Wno-unused-but-set-variable -Wno-strict-aliasing
CFLAGS              += -I$(PROJECT_DIR) -I.

vpath %.c $(PROJECT_DIR)/IceC
vpath %.c $(PROJECT_DIR)/IceC/platforms/z1

all: $(SLICE:%.ice=%.h) $(APP)

show-log: reset login

upload: $(APP).upload

reset: z1-reset

%.h: %.ice
	$(PROJECT_DIR)/bin/slice2c $^

.PHONY: vclean
vclean: clean
	$(RM) $(SLICE:%.ice=%.h)

include $(CONTIKI)/Makefile.include
