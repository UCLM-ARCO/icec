# -*- mode: makefile-gmake; coding: utf-8 -*-

PROJECT_DIR = ../../
CFLAGS      = -DESP8266 -I $(PROJECT_DIR)
TARGET      = writer_client

include esp8266.mk


.PHONY: vclean
vclean: clean
	$(RM) -r *~
