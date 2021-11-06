# Installation

Wie man diese Idee installiert

## Install docker for HA

HA -> Home Assisten
In der ersten Version werden wir das wohl dringend brauchen später sollten die meisten Treiber wo hier sein
Dann ist das "nice to Have" oder halt auch nicht

in ein Verzeichnis wechsel wo der Install script hin so zum Beispiel ~/Downloads oder so

curl -fsSL https://get.docker.com -o get-docker.sh
Holt das docker install script im Netz

DRY_RUN=1 sh ./get-docker.sh
Ansehen was das Script macht

sudo sh ./get-docker.sh
Docker installieren

sudo groupadd docker
sudo usermod -aG docker <benutzer name> pi bei standard
newgrp docker

docker run hello-world <-Ersten Docker Container installieren und so testen obs läuft

https://docs.docker.com/engine/install/debian/#install-using-the-convenience-script

### Container auf die Festplatte

sudo systemctl stop docker -> Service beenden
sudo rsync -axPS /var/lib/docker/ /media/hddIntern/docker -> alle daten rüber schieben
sudo nano /lib/systemd/system/docker.service -> Service File bearbeiten
--data-root /media/hddIntern/docker -> in den Start auf ruf bastel
ExecStart=/usr/bin/dockerd --data-root /media/hddIntern/docker -H fd:// --containerd=/run/containerd/containerd.sock <- so könnte der dann aus sehen

sudo systemctl daemon-reload
sudo systemctl start docker
docker info | grep "Root Dir"

sudo rm -r /var/lib/docker/ -> den platz wollen wir ja nicht verschenken

https://stackoverflow.com/questions/55344896/attempt-to-change-docker-data-root-fails-why

## HA Installieren

mkdir homeassistant

Install:

docker run -d \
  --name homeassistant \
  --privileged \
  --restart=unless-stopped \
  -e TZ=MY_TIME_ZONE \
  -v /media/hddIntern/homeassistant:/config \
  --network=host \
  ghcr.io/home-assistant/raspberrypi4-homeassistant:stable  < aufpassen richtige Image nehmen 3 oder 4

Update:

docker pull ghcr.io/home-assistant/raspberrypi4-homeassistant:stable
if this returns "Image is up to date" then you can stop here

docker stop homeassistant

docker rm homeassistant < remove it from Docker's list of containers
reinstall container

https://www.home-assistant.io/installation/raspberrypi

http://192.168.2.99:8123

## Todo

Docker as a non-root user
