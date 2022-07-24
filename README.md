# This is an Test Projekt for Mqtt Protokol and Raspberry PI

## What it does or not

The Build an C++ Webserver that Connect to an MqttServer (mosquitto).

The Configfile is in bin/linux after First Run

Enter in your Browser localhost:8080

[more](documents/Konzept.md)

## Dependency

sieh [DebianPackages](DebianPackages.txt) and [PythonPackages](PythonPackages.txt)

## Develop

I developing with Visual Studio Code

add --v=9 to command for log everythink

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

[Index](documents/Dokumentation/Index.md)

## Build

change in your Develop path and run
cd /media/hddIntern/devtest

Build script is written for Debian based Linux (Test only with rasbian or debian)

```bash
wget -O build.sh https://raw.githubusercontent.com/DerKleinePunk/AnOtherAutomationServer/master/build.sh
chmod +x build.sh
./build.sh
```

```bash
cd AnOtherAutomationServer/bin/Linux/
./SimpelIoBackend.bin
```
