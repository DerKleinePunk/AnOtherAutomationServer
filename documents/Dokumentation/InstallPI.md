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

[install-using-the-convenience-script](https://docs.docker.com/engine/install/debian/#install-using-the-convenience-script)

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

[attempt-to-change-docker-data-root-fails-why](https://stackoverflow.com/questions/55344896/attempt-to-change-docker-data-root-fails-why)

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

[HA on PI](https://www.home-assistant.io/installation/raspberrypi)

[Local Link](http://192.168.2.99:8123)

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

## Network Manager

Die Implementierung bassiert auf [wifi-HotSpot](https://github.com/MartinVerges/wifi-HotSpot)

To Controller the with this Software braucht man den Netwerk Manager

```bash
sudo apt-get install network-manager network-manager-config-connectivity-debian
sudo apt purge openresolv dhcpcd5
```

Das Remove braucht man das mit der NM das WLAN Verwalten kann.
[Headless-Setup-via-Network-Manager](https://awesomeopensource.com/project/sraodev/Raspberry-Pi-Headless-Setup-via-Network-Manager)

### Network Manager Console Komands

nmcli radio
nmcli device

### nstall DHCP and DNS for AP

```bash
sudo apt-get install dnsmasq
```

```text
interface=wlan0,eth0
no-dhcp-interface=eth1,wlan1
dhcp-range=interface:wlan0,192.168.255.101,192.168.255.199,255.255.255.0,14d
dhcp-range=interface:eth0,172.29.0.101,172.29.0.199,255.255.255.0,14d
proxy-dnssec
cache-size=1000
domain-needed
bogus-priv

server=8.8.8.8
server=1.1.1.1
server=208.67.222.222
```

In short, I offer DHCP and DNS on wlan0 (integrated) and eth0 (network port). DNS is proxied to 8.8.8.8 (Google) or 1.1.1.1 (Cloudflare) or 208.67.222.222 (OpenDNS).

## Bios Update

[updating-the-bootloader](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#updating-the-bootloader)

```bash
sudo rpi-eeprom-update
```

```bash
sudo rpi-eeprom-update -a
```

## Todo

Docker as a non-root user
