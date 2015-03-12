# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR     = ../../
ICEC_SRC    = $(BASEDIR)/IceC
SLICE2C     = $(BASEDIR)/bin/slice2c

CFLAGS      = -DESP8266 -I$(BASEDIR)
TARGET      = writer_client
OBJECTS     = $(TARGET).o IceC.o port.o
# IceUtil.o UDPEndpoint.o 
SLICE       = hello.ice

vpath %.c $(ICEC_SRC)
vpath %.c $(ICEC_SRC)/platforms/esp8266/

include esp8266.mk


$(TARGET).o: $(SLICE:%.ice=%.h) $(TARGET).c 

$(SLICE:%.ice=%.h): $(SLICE)
	$(RM) -rfv $@
	$(SLICE2C) $^

.PHONY: vclean
vclean: clean
	$(RM) -r *~ $(SLICE:%.ice=%.h)

