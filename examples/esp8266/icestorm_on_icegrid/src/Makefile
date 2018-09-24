# -*- mode: makefile-gmake; coding: utf-8 -*-

BASEDIR  = ../../../..
SLICE2C  = $(BASEDIR)/bin/slice2c

SLICES   = example.ice IceStorm-lite.ice
REG_HOME = /tmp/test-registry
NOD_HOME = /tmp/test-node

all: $(SLICES:%.ice=%.h)

%.h: %.ice
	$(RM) -rf $@
	$(SLICE2C) --ice -I/usr/share/slice $^

run-icegrid: DETACH ?= gnome-terminal --
run-icegrid: run-registry run-node load-application run-subscriber

run-registry:
	$(RM) -r $(REG_HOME)
	mkdir -p $(REG_HOME)
	$(DETACH) icegridregistry --Ice.Config=registry.config

run-node:
	sleep 2
	$(RM) -r $(NOD_HOME)
	mkdir -p $(NOD_HOME)/node1
	$(DETACH) icegridnode --Ice.Config=node.config

load-application:
	sleep 2
	icegridadmin -u user -p pass -H 192.168.4.2 -e "application add application.xml"

run-subscriber:
	sleep 5
	$(DETACH) ./subscriber.py


.PHONY: clean
clean:
	$(RM) $(SLICES:%.ice=%.h) *.o *~
	$(RM) -r $(REG_HOME) $(NOD_HOME)
