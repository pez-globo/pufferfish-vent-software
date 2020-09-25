#!/bin/bash

echo "********** Setting up backend service **********"

# Copying service file to systemd
cd ~/pufferfish-vent-software/deploy

if [ 1 -eq $( ls configs/ | grep -c "pufferfish_backend.service" ) ]
then
    sudo cp configs/pufferfish_backend.service /etc/systemd/system/
    sudo chmod 644 /etc/systemd/system/pufferfish_backend.service
else
    echo "The pufferfish_backend.service file doesn't exist"
    exit 1
fi

# Enabling service
sudo systemctl daemon-reload
sudo systemctl enable pufferfish_backend.service