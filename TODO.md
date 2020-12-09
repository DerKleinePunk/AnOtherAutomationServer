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

## Test Commands
mosquitto_pub -h 192.168.2.99 -t "test/message" -m "Hello, from Desktop"
