# Todos an Infos for the Home Server Project

## Mqtt

### PI

sudo apt install mosquitto mosquitto-clients
sudo systemctl enable mosquitto
sudo apt-get install apache2 apache2-doc php php-pear
sudo apt-get install libsystemd-dev libcurl4-openssl-dev

https://github.com/mschlenstedt/Loxberry/issues/952

### Desktop

sudo apt install mosquitto-clients
sudo apt install libmosquitto-dev

## Shelly

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
https://techtutorialsx.com/2021/11/02/esp32-dynamic-sensor-network/

## Test Commands

mosquitto_pub -h 192.168.2.99 -t "test/message" -m "Hello, from Desktop"

mosquitto_pub -h 192.168.2.99 -t "devices/wb-adc/controls/text" -m "Hello, from Desktop"
mosquitto_pub -h 192.168.2.49 -t "SimpleIo/Variable/hofbel" -m "ON"
mosquitto_pub -h 192.168.2.49 -t "SimpleIo/Variable/notify" -m "ON"

whatch all

```bash
mosquitto_sub -h 192.168.2.99 -t \# -d
mosquitto_sub -h 192.168.2.49 -t \# -d
```

mosquitto_sub -h 192.168.2.99 -t devices/# -d

cmnd/tasmota_switch/Power TOGGLE

```bash
mosquitto_pub -h 192.168.2.49 -t "cmnd/tasmota_852612/POWER" -m "TOGGLE"
mosquitto_pub -h 192.168.2.49 -t "cmnd/tasmota_852612/POWER2" -m "TOGGLE" or ON / OFF
mosquitto_pub -h 192.168.2.49 -t "shellies/shelly1Tor/relay/0/command" -m "toggle" or on / off
mosquitto_pub -h 192.168.2.49 -t "herbiOs/lights/Licht1/state" -m "ON"
mosquitto_pub -h 192.168.2.49 -t "herbiOs/lights/Licht1/state" -m "{ \"state\" : \"ON\", \"brightness\" : 125}"
mosquitto_pub -h 192.168.2.49 -t "mn/test/1" -m "TestValue"
mosquitto_pub -h 192.168.2.49 -t "SimpleIo/Io/RelaisBoard" -m "on"
mosquitto_pub -h 192.168.2.50 -t "SimpleIo/Io/RelaisBoard" -m "on"
mosquitto_pub -h 192.168.2.50 -t "herbiOs/switches/inverter/config" -m "{ \"name\": \"Inverter\", \"unique_id\": \"inverter\" }"
```

HerbiOs default password hMA3x994QvC46YZKnlBnnhZy4QAsQJus

herbiOs/switches/inverter/config
{ "name": "Inverter", "unique_id": "inverter" }

nmcli device wifi connect ROBOTERNETZ

```Json
{"ip":"192.168.X.Y","dn":"Tasmota","fn":["Tasmota","","","",null,null,null,null],"hn":"tasmota-852612-1554","mac":"84CCA8852612","md":"4CH Relay","ty":0,"if":0,"ofln":"Offline","onln":"Online","state":["OFF","ON","TOGGLE","HOLD"],"sw":"10.0.0","t":"tasmota_852612","ft":"%prefix%/%topic%/","tp":["cmnd","stat","tele"],"rl":[1,1,1,1,0,0,0,0],"swc":[-1,-1,-1,-1,-1,-1,-1,-1],"swn":[null,null,null,null,null,null,null,null],"btn":[0,0,0,0,0,0,0,0],"so":{"4":0,"11":0,"13":0,"17":0,"20":0,"30":0,"68":0,"73":0,"82":0,"114":0,"117":0},"lk":0,"lt_st":0,"sho":[0,0,0,0],"ver":1}
```

## mDNS

https://www.megunolink.com/documentation/connecting/mdns-browser/
apt-get install avahi-utils
libavahi-client-dev
https://www.avahi.org/doxygen/v0.7/html/client-browse-services_8c-example.html

http://192.168.1.2/cm?cmnd=setoption55%201 Enable mDNS on Tasmota
Funktioniert nicht mangels Resourcen im Chip !?

https://www.sigmdel.ca/michel/ha/rpi/syslog_en.html

http://arduinoarsingabout.blogspot.com/2018/02/mdns-sonoff-tasmota-code-spelunking-and.html

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

## Git hints

git reset --hard
git clean -fx
git checkout feature/4-mqtt-rewrite
git submodule add https://github.com/DerKleinePunk/PiGPIOHelper.git

## Gauge

https://github.com/Mikhus/canvas-gauges
https://www.bauer.uh.edu/parks/h5_gauge1.htm

## NodeJs Hints

nvm install --lts
nvm use --lts

## Flutter Links

https://sanjibsinha.com/notification-class-in-flutter/
Big Problem No Build on Rasperry 32 Bit

## Victoria Metrics for Hostrie Data

docker pull victoriametrics/victoria-metrics -> Container get

docker run -it --rm -v /path/to/victoria-metrics-data:/victoria-metrics-data -p 8428:8428 victoriametrics/victoria-metrics -> Start Container
docker run -it --rm -v /home/punky/data/victoria-metrics-data:/victoria-metrics-data -p 8428:8428 victoriametrics/victoria-metrics -> Start Container
http://localhost:8428/metrics -> Get Metrics

Write Some data
curl -d 'measurement,tag1=value1,tag2=value2 field1=123,field2=1.23' -X POST 'http://localhost:8428/write'

https://grafana.com/docs/grafana/latest/setup-grafana/installation/debian/

http://localhost:3000

## PI Links

https://gist.github.com/meinside/0538087cc60a2f0654bb