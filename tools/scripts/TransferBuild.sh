#Remember to use ssh-keyscan <SERVER> > ~/.ssh/known_hosts to persist the SFTP server fingerprint to avoid interactive prompting

SFTP_SERVER=$RPI_IP
SFTP_USER=$RPI_USER
SFTP_PWD=$RPI_PASSWORD
currentdir=$(pwd)

if [[ -z "$SFTP_SERVER" ]]; then
   SFTP_SERVER="192.168.2.51"
fi

if [[ -z "$SFTP_USER" ]]; then
   SFTP_USER="pi"
fi

if [[ -z "$SFTP_PWD" ]]; then
   SFTP_PWD="raspberry"
fi

pscp -batch -pw "$SFTP_PWD" $currentdir/bin/PI/TestApp.bin pi@$SFTP_SERVER:/home/pi/devtest/ops4bos/bin/PI/
pscp -batch -pw "$SFTP_PWD" $currentdir/tools/pipower/pi-power-service.py pi@$SFTP_SERVER:/home/pi/devtest/ops4bos/tools/pipower/
pscp -batch -pw "$SFTP_PWD" $currentdir/run.sh pi@$SFTP_SERVER:/home/pi/devtest/ops4bos/
