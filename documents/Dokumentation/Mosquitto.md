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
```

### logging.conf

```text
#log_type error
#log_type warning
log_type all
log_dest stdout
```

### listener.conf

```text
listener 1883
allow_anonymous true
# password_file /etc/mosquitto/password_file
```

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
