#!/bin/bash

echo "********** Setting up Watchdog **********"

# Loading internal watchdog kernel module
sudo modprobe bcm2835_wdt
echo "bcm2835_wdt" | sudo tee -a /etc/modules

# Installing watchdog
sudo apt-get install watchdog -y

# Adding watchdog config
cd ~/pufferfish-vent-software/deploy
cat configs/pyenv_config.txt | sudo tee -a /etc/watchdog.conf

# Starting watchdog service
sudo systemctl start watchdog
sudo systemctl daemon-reload
