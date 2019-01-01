# ESP8266-WiFi-Socket-NG

Alternative Firmware for `https://www.obi.de/hausfunksteuerung/wifi-stecker-schuko/p/2291706`

## New OBI Wifi Socket since late 2018

Find the new repository for new OBI socket [here](https://github.com/thorsten-l/ESP8266-WiFi-Socket-II).

## Precompiled Firmware

### Attention - update OTA from 1.1.x to 1.2.x

> If you like to update from firmware 1.1.x to 1.2.x over the air (OTA) please first flash firmware version 1.2.0migration.
>
> If you try to update direct from 1.1.x to the current 1.2.x firmware
version you will get an "out of memory" error
>
> Due to a new memory model the firmware will reset all settings to the default values

Find the precompiled firmware in the [firmware](https://github.com/thorsten-l/ESP8266-WiFi-Socket-NG/tree/master/firmware) directory.

## Compile

This is a [PlatformIO](https://platformio.org/) project.

## Firmware Reset

1. Power on the WiFi Socket (plug in)
2. Press down and hold the Power-Button during the first 5s
3. Release Power-Button when the WiFi LED starts rapidly fast blinking
4. The firmware reset ends after about 30s indicated by a permanent WiFi LED on.

## Default Settings

After a firmware reset the following values are set.

|    | value | description |
|----|-------|-------------|
| Admin password | admin | Setup admin password |
| WiFi Mode | AP | Accesspoint- or Station-Mode |
| WiFi SSID | WifiSocketII-`esp id` | SSID in accesspoint mode |
| WiFi Password | 12345678 | Default WiFi password |
| Network Mode | DHCP | Network mode STATIC or DHCP *1 |
| Network IP-Address | 192.168.192.1 | device ip address *2 |
| Network Mask | 255.255.255.0 | Subnet mask *2 |
| Network Gateway | 192.168.192.1 | Default gateway *2 |
| Network DNS | 192.168.192.1 | DNS server address *2 |
| OTA Hostname | wifi-socket-1 | *O*ver *T*he *A*ir hostname |
| OTA Password | otapass | OTA firmware upload password |
| OpenHAB Callback Enabled | **false** | |
| OpenHAB Version | 1 | Callback for OpenHAB version 1 or 2 |
| OpenHAB Item Name | WifiSocket1 | |
| OpenHAB Host | 192.168.1.1 | |
| OpenHAB Port | 80 | |
| OpenHAB Use Authentication | false | |
| OpenHAB User | user |  |
| OpenHAB Password | password | |
| Alexa enabled | **false** |  |
| Alexa devicename | Socket | Devicename for Alexa HA |
| MQTT Enabled | **false** |  |
| MQTT ClientId | WifiSocket1 |  |
| MQTT Host | 192.168.1.1 | |
| MQTT Port | 1883 | |
| MQTT Use Authentication | false | |
| MQTT User | user |  |
| MQTT Password | password | |
| MQTT In Topic | socket1/in | receiving commands |
| MQTT Out Topic | socket1/out | sending callback |
| Syslog Enabled | **false** |  |
| Syslog Host | 192.168.1.1 | |
| Syslog Port (TCP) | 514 | |
| Syslog App Name | gateway-doorbell |  |

*1 In WiFi Station-Mode only

*2 will be overwritten from DHCP-Response

## Hardware

[OBI Wifi Stecker Schuko](https://www.obi.de/hausfunksteuerung/wifi-stecker-schuko/p/2291706)

## OpenHAB

### OpenHAB - HTTP Binding sample

```t
Switch LivingRoom_Power "Living Room Outlet" {http=">[ON:GET:http://wifi-socket.ip/on] >[OFF:GET:http://wifi-socket.ip/off]"}
```

If you like to read the state of the socket by OpenHAB. First create a file `getState.js` in the `transform` directory with the following content:

```javascipt
JSON.parse(input).state;
```

add

```t
<[http://wifi-socket.ip/state:30000:JS(getState.js)]
```

to the `Switch` definition. Every 30s the state of the socket will be checked.

### OpenHAB - MQTT binding sample

```t
Switch LivingRoom_Power "Living Room Outlet" {mqtt=">[broker1:socket1/in:command:*:default], <[broker1:socket1/out:state:default]"}
```

## References

- [PlatformIO](https://platformio.org/)
- [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
- [Async TCP Library for ESP8266](https://github.com/me-no-dev/ESPAsyncTCP)
- [Async Web Server for ESP8266 and ESP32](https://github.com/me-no-dev/ESPAsyncWebServer)
- [FauxmoESP - Amazon Alexa support for ESP8266 and ESP32 devices](https://bitbucket.org/xoseperez/fauxmoesp)
- [Arduino-pubsubclient - A client library for the ESP8266 that provides support for MQTT](https://github.com/knolleary/pubsubclient)
- [Pure.CSS - A nice CSS, as GZIP it is less than 4k so it is useful for embedded devices](https://purecss.io/)
