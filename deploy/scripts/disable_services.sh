#!/bin/bash

# Disables services and their dependencies
# Services include bluetooth, bluetooth helpers
# Daily update and upgrade services, printer and smartcard

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Disabling unnecessary services **********\n${NC}"

# Masks all the unnecessary services
# Removes its dependencies

sudo systemctl mask bluetooth.service
sudo systemctl mask hciuart.service
sudo systemctl mask bluealsa.service
sudo systemctl mask bthelper@.service
sudo systemctl mask apt-daily-upgrade.service
sudo systemctl disable apt-daily-upgrade.timer
sudo systemctl mask apt-daily.service
sudo systemctl disable apt-daily.timer
sudo systemctl mask bluetooth.target
sudo systemctl mask printer.target
sudo systemctl mask smartcard.target

# Remove bluetooth dependencies
sudo apt-get purge bluez piwiz -y
sudo apt-get autoremove --purge -y

# Disable default ssh password warning
if [ 1 -eq $( ls /etc/xdg/lxsession/LXDE-pi/ | grep -c "sshpwd.sh" ) ]
then
    sudo rm /etc/xdg/lxsession/LXDE-pi/sshpwd.sh
fi

# Add configuration to disable bluetooth
if [ 0 -eq $( grep -c "^dtoverlay=disable-bt" /boot/config.txt ) ]
then
    echo -e "\ndtoverlay=disable-bt" | sudo tee -a /boot/config.txt
else
    echo -e "${WARNING} Bluetooth is already disabled${NC}"
fi

sudo systemctl daemon-reload

echo -e "\n${SUCCESS}Unnecessary Services disabled\n${NC}"