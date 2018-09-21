#!/bin/bash
# -*- mode: sh; coding: utf-8 -*-

echo -n "IceC-IoT-Node... "
if [ ! -d IceC-IoT-Node ]; then
    wget -q https://bitbucket.org/arco_group/iot.node/downloads/icec-iot-node-latest.zip
    unzip -qq icec-iot-node-latest.zip -d IceC-IoT-Node
    rm icec-iot-node-latest.zip
    echo "[ok] "
else
    echo "[already present]"
fi