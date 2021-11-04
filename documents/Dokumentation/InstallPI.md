# Install docker for HA

in ein verzeichnis wechsel wo der Install script hin so zum Beispiel ~/Downloads oder so

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

# Todo
Docker as a non-root user