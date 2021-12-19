# Hinweise zu Mosquitto

## Allgemein

Ab der Version 2 starte der Server perfault nur im Local Only mode das heist daten on Anmelden
geht nur über den Local Host.

[authentication methods](https://mosquitto.org/documentation/authentication-methods/)

Unter Debian kann man hier /etc/mosquitto/conf.d/ seine Configs ab legen
Es werden alle dateinen die auf .conf enden ein gelesen

Das sind Config Files wie ich sie mir überlegt habe man könnte auch alles in eine schreiben

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

## Links

[mosquitto-logging](http://www.steves-internet-guide.com/mosquitto-logging/)
[Python und Mqtt](https://smarthome-blogger.de/blog/tutorial/python-mqtt-tutorial/)
