# This is an Test Projekt for Mqtt Protokol and Raspberry PI

## What it does or not

The Build an C++ Webserver that Connect to an MqttServer (mosquitto).

The Configfile is in bin/linux after First Run

Enter in your Browser localhost:8080

## Develop

I developing with Visual Studio Code

### VSCode

settings.json

```JSON settings.json
"cmake.configureSettings": { 
        "ENABLE_CPPCHECK" : "OFF",
        "TARGET" : "Linux"
    }
```
### Todo

[Todo](TODO.md)

## Documentation (German)

[concept](documents/Konzept.md)

## Build

change in your Develop path and run

```bash
wget -O build.sh https://raw.githubusercontent.com/DerKleinePunk/AnOtherAutomationServer/master/build.sh
chmod +x build.sh
```