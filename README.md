# ESP8266-WiFi-Socket-NG
Alternative Firmware for https://www.obi.de/hausfunksteuerung/wifi-stecker-schuko/p/2291706

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
| WiFi SSID | WifiSocket-`esp id` | SSID in accesspoint mode |
| IP Address | 192.168.192.1 | ip address in accesspoint mode |
| Admin password | admin | Setup admin password |
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
| MQTT Port | 80 | |
| MQTT Use Authentication | false | |
| MQTT User | user |  |
| MQTT Password | password | |
| MQTT In Topic | socket1/in | receiving commands |
| MQTT Out Topic | socket1/out | sending callback |


## Hardware
[OBI Wifi Stecker Schuko](https://www.obi.de/hausfunksteuerung/wifi-stecker-schuko/p/2291706)

## OpenHAB 1.x - HTTP Binding sample

```
Switch pOD_Licht "Ein / Aus" {http=">[ON:GET:http://wifi-socket.ip/on] >[OFF:GET:http://wifi-socket.ip/off]"}
```

If you like to read the state of the socket by OpenHAB. First create a file `getState.js` in the `transform` directory with the following content:
```
JSON.parse(input).state;
```

add
```
<[http://wifi-socket.ip/state:30000:JS(getState.js)]
```
to the `Switch` definition. Every 30s the state of the socket will be checked.
