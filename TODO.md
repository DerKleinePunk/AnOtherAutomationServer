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

192.168.33.X
192.168.2.91 Tor

https://shelly-api-docs.shelly.cloud

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

mosquitto_sub -h 192.168.2.99 -t devices/# -d

## mDNS

https://www.megunolink.com/documentation/connecting/mdns-browser/

## http  Websocket

MHD_ALLOW_SUSPEND_RESUME

https://github.com/mnisjk/cppWebSockets
https://git.gnunet.org/libmicrohttpd.git/tree/src/examples/websocket_chatserver_example.c