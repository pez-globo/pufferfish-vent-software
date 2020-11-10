#!/bin/bash

# Creates a systemd service for starting browser in kiosk mode on boot
# Adds a script to reset config before starting browser

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up kiosk service **********\n${NC}"

# Adding script to clean kiosk failure config
if [ 1 -eq $( ls $config_dir | grep -c "clean_kiosk.sh" ) ]
then
    sudo mkdir -p /opt/pufferfish
    sudo cp $config_dir/clean_kiosk.sh /opt/pufferfish/
    sudo chmod +x /opt/pufferfish/clean_kiosk.sh
else
    echo -e "${ERROR} The clean_kiosk.sh file doesn't exist${NC}"
    exit 1
fi

# Copy target file to systemd directory
if [ 1 -eq $( ls $config_dir | grep -c "pufferfish.target" ) ]
then
    sudo cp $config_dir/pufferfish.target /etc/systemd/system/
    sudo chmod 644 /etc/systemd/system/pufferfish.target
else
    echo -e "${ERROR} The pufferfish.target file doesn't exist${NC}"
    exit 1
fi

if [ 1 -eq $( ls $config_dir | grep -c "kiosk.service" ) ]
then
    sudo cp $config_dir/kiosk.service /etc/systemd/system/
    sudo chmod 644 /etc/systemd/system/kiosk.service
else
    echo -e "${ERROR} The kiosk.service file doesn't exist${NC}"
    exit 1
fi

# Enabling service
sudo systemctl daemon-reload
sudo systemctl set-default pufferfish.target
sudo systemctl enable kiosk.service

echo -e "\n${SUCCESS}Kiosk setup complete\n${NC}"