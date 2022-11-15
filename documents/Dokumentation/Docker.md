## Enable daemon.json with Systemd

sudo mkdir /etc/systemd/system/docker.service.d
sudo nano /etc/systemd/system/docker.service.d/override.conf
[Service]
ExecStart=
ExecStart=/usr/bin/dockerd

sudo systemctl daemon-reload
sudo systemctl start docker

sudo dockerd -> Run docker in console to test the config

docker image prune -a -> clear all

docker info -> Infos
