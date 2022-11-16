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

https://www.blog.berrybase.de/blog/2022/02/23/docker-auf-dem-raspberry-pi-basics/
https://docs.portainer.io/start/install/server/setup

/boot/cmdline.txt

cgroup_enable=memory cgroup_memory=1 swapaccount=1

https://github.com/me-box/databox/issues/303