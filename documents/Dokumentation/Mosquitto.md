# Hinweise zu Mosquitto

## Allgemein

Ab der Version 2 starte der Server perfault nur im Local Only mode das heist daten on Anmelden
geht nur über den Local Host.

[authentication methods](https://mosquitto.org/documentation/authentication-methods/)

Unter Debian kann man hier /etc/mosquitto/conf.d/ seine Configs ab legen
Es werden alle dateinen die auf .conf enden ein gelesen

Das sind Config Files wie ich sie mir überlegt habe man könnte auch alles in eine schreiben

```bash
sudo nano /etc/mosquitto/conf.d/logging.conf
sudo nano /etc/mosquitto/conf.d/listener.conf
sudo nano /etc/mosquitto/conf.d/security.conf
```

### logging.conf

```text
#log_type error
#log_type warning
log_type all
log_dest stdout
log_dest /media/hddIntern/mqtt/mosquitto.log
```

### listener.conf

```text
listener 1883
allow_anonymous false
persistence true
persistence_location /media/hddIntern/mqtt/
```

### security

```text
allow_zero_length_clientid false
password_file /media/hddIntern/mqtt/password_file
```

User anlegen

```bash
sudo mosquitto_passwd -c /media/hddIntern/mqtt/passwd HomeMqtt
```

Leserechte für alle auf der passwort datei
```bash
chmod a+r password_file
chown -R pi:users /media/hddIntern/mqtt
sudo usermod -aG users root
```
### Config Testen

sudo mosquitto -v -c /etc/mosquitto/mosquitto.conf
sudo journalctl --unit mosquitto -f

## Clear mosquitto

```bash
sudo service mosquitto stop
sudo rm /var/lib/mosquitto/mosquitto.db
sudo service mosquitto start
```

## Links

[mosquitto-logging](http://www.steves-internet-guide.com/mosquitto-logging/)
[Python und Mqtt](https://smarthome-blogger.de/blog/tutorial/python-mqtt-tutorial/)
[deleting-mosquitto-mqtt-retained-messages](https://andygoodenberger.wordpress.com/2017/09/24/deleting-mosquitto-mqtt-retained-messages/)
