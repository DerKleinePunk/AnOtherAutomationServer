# Installation

Wie man diese Idee installiert

## Install docker for HA

HA -> Home Assisten
ist "nice to Have" bzw Dokumentiert um Dinge Testen zu können
Und machmal Idee sich auch ändern können

in ein Verzeichnis wechsel wo der Install script hin so zum Beispiel ~/Downloads oder so

```bash
curl -fsSL https://get.docker.com -o get-docker.sh
REM Holt das docker install script im Netz

DRY_RUN=1 sh ./get-docker.sh
REM Ansehen was das Script macht

sudo sh ./get-docker.sh
REM Docker installieren

sudo groupadd docker
sudo usermod -aG docker <benutzer name> pi bei standard
newgrp docker

docker run hello-world 
REM Ersten Docker Container installieren und so testen obs läuft
```

https://docs.docker.com/engine/install/debian/#install-using-the-convenience-script

### Container auf die Festplatte

```bash
sudo systemctl stop docker 
REM Service beenden

sudo rsync -axPS /var/lib/docker/ /media/hddIntern/docker -> REM alle daten rüber schieben

sudo nano /lib/systemd/system/docker.service 
REM Service File bearbeiten
REM --data-root /media/hddIntern/docker -> in den Start auf REM ruf bastel
REM ExecStart=/usr/bin/dockerd --data-root /media/hddIntern/REM docker -H fd:// --containerd=/run/containerd/containerd.REM sock <- so könnte der dann aus sehen

sudo systemctl daemon-reload
sudo systemctl start docker
docker info | grep "Root Dir"

sudo rm -r /var/lib/docker/ -> den platz wollen wir ja nicht verschenken
```

https://stackoverflow.com/questions/55344896/attempt-to-change-docker-data-root-fails-why

### HA Container Installieren

mkdir homeassistant

Install:

```bash
docker run -d \
  --name homeassistant \
  --privileged \
  --restart=unless-stopped \
  -e TZ=MY_TIME_ZONE \
  -v /media/hddIntern/homeassistant:/config \
  --network=host \
  ghcr.io/home-assistant/raspberrypi4-homeassistant:stable  < aufpassen richtige Image nehmen 3 oder 4
```

Update:

docker pull ghcr.io/home-assistant/raspberrypi4-homeassistant:stable
if this returns "Image is up to date" then you can stop here

docker stop homeassistant

docker rm homeassistant < remove it from Docker's list of containers
reinstall container

https://www.home-assistant.io/installation/raspberrypi

http://192.168.2.99:8123

## Mqtt

sudo apt install mosquitto mosquitto-clients
sudo systemctl enable mosquitto

[Hier weiter](Mosquitto.md)

## I²C

```bash
sudo apt-get install i2c-tools
i2cdetect -y 1
```

Welche Geräte Sind am Bus

sudo echo 6 >/sys/class/gpio/export
sudo echo out > /sys/class/gpio/gpio6/direction
sudo echo 1 >/sys/class/gpio/gpio6/value

Der GPIO 6 ist bei mir die Freigabe von MCP23017 damit bei Startup nix dummes passiert.

sudo adduser pi gpio

## Todo

Docker as a non-root user
