# Todos an Infos for the Home Server Project

## Mqtt

### PI

sudo apt install mosquitto mosquitto-clients
sudo systemctl enable mosquitto
sudo apt-get install apache2 apache2-doc php php-pear
sudo apt-get install libsystemd-dev libcurl4-openssl-dev libmicrohttpd-dev

### Desktop

sudo apt install mosquitto-clients
sudo apt install libmosquitto-dev

### Shelly

192.168.33.X default
192.168.2.91 Tor

https://shelly-api-docs.shelly.cloud

https://shelly-api-docs.shelly.cloud/gen1/#shelly1-1pm-mqtt

## Links

https://www.systutorials.com/docs/linux/man/3-libmosquitto/
https://github.com/warmcat/libwebsockets
https://github.com/adamrehn/websocket-server-demo
https://www.heise.de/developer/artikel/Kommunikation-ueber-MQTT-3238975.html
https://www.kohlenklau.de/mqtt-auf-raspberry-pi-installieren/
https://www.sophos.com/en-us/medialibrary/PDFs/documentation/UTMonAWS/Sophos-UTM-RESTful-API.pdf
https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT
https://www.waveshare.com/w/upload/d/d8/2.13inch_e-Paper_%28B%29_V3_Specification.pdf

## Test Commands

mosquitto_pub -h 192.168.2.99 -t "test/message" -m "Hello, from Desktop"

mosquitto_pub -h 192.168.2.99 -t "devices/wb-adc/controls/text" -m "Hello, from Desktop"

whatch all
mosquitto_sub -h 192.168.2.99 -t \# -d
mosquitto_sub -h 192.168.2.49 -t \# -d

mosquitto_sub -h 192.168.2.99 -t devices/# -d

cmnd/tasmota_switch/Power TOGGLE

mosquitto_pub -h 192.168.2.49 -t "cmnd/tasmota_852612/POWER" -m "TOGGLE"
mosquitto_pub -h 192.168.2.49 -t "cmnd/tasmota_852612/POWER2" -m "TOGGLE" or ON / OFF
mosquitto_pub -h 192.168.2.49 -t "shellies/shelly1Tor/relay/0/command" -m "toggle" or on / off

## mDNS

https://www.megunolink.com/documentation/connecting/mdns-browser/

## http  Websocket

MHD_ALLOW_SUSPEND_RESUME

https://github.com/mnisjk/cppWebSockets
https://git.gnunet.org/libmicrohttpd.git/tree/src/examples/websocket_chatserver_example.c

## Victron

https://www.victronenergy.com/live/vedirect_protocol:faq
https://github.com/karioja/vedirect
https://github.com/cterwilliger/VeDirectFrameHandler
https://community.victronenergy.com/questions/63915/anyone-have-python-example-how-to-read-mqtt-values.html

## Automatik Build
https://medium.com/swlh/how-to-create-an-automated-build-pipeline-for-your-arduino-project-1df9826f2a5e
