Description
===========

Minimal implementation of [ZeroC Ice](http://zeroc.com) middleware in ANSI C.

Features
========

Endpoint         | TCP | UDP | XBeeAT | FIFO
-----------------|-----|-----|--------|-------
oneway server    | WIP | NA  | YES    | YES
oneway client    | WIP | NA  | YES    | YES
twoway server    | YES | NA  | WIP    | NO
twoway client    | WIP | NA  | WIP    | NO
datagram server  | NA  | YES | NA     | NA
datagram client  | NA  | YES | NA     | NA

**YES**: does have support
**NO**: currently does not have support
**WIP**: work in progress to add this feature
**NA**: not applicable