#!/bin/sh
$HOME/.platformio/penv/bin/python \
  $HOME/.platformio/packages/tool-espotapy/espota.py \
  --debug --progress -i "wifi-socket-1.local" \
  --auth="otapass" -f "firmware-1.1.5.bin"
