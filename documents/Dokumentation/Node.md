https://tecadmin.net/how-to-install-nvm-on-debian-11/

n als node versioniere funktioniert unter debian nicht wirklich

```bash

sudo apt install curl 
curl https://raw.githubusercontent.com/creationix/nvm/master/install.sh | bash
source ~/.bashrc oder abmelden und wieder an melden

nvm install --lts 
nvm use --lts
nvm alias default lts/*
```
