# -*- mode: makefile-gmake; coding: utf-8 -*-

DETACH ?= gnome-terminal --

all:
	slice2c /usr/share/slice/citisim/iot.ice
	slice2c /usr/share/slice/idm/idm.ice --iface IDM_NeighborDiscovery_Listener

run-all: run-serial-service delay run-idm-router

run-idm-router:
	$(DETACH) idm-router --Ice.Config=router.config

run-serial-service:
	$(DETACH) serial-service --Ice.Config=serial-service.config

run-server:
	$(DETACH) python3 server.py | tee proxy.out

run-client:
	./client.py 'A00A0002 -t:tcp -h 127.0.0.1 -p 6140' $(IMAGE)

delay:
	sleep 1


.PHONY: clean
clean:
	$(RM) -f iot.h idm.h *~ *.table
