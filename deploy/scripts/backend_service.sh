#!/bin/bash

# Backend systemd service to run backend on boot

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up backend service **********\n${NC}"

# Copy target file to systemd directory
if [ 1 -eq $( ls $config_dir | grep -c "pufferfish.target" ) ]
then
    sudo cp $config_dir/pufferfish.target /etc/systemd/system/
    sudo chmod 644 /etc/systemd/system/pufferfish.target
else
    echo -e "${ERROR} The pufferfish.target file doesn't exist${NC}"
    exit 1
fi

# Copy service file to systemd directory
if [ 1 -eq $( ls $config_dir | grep -c "pufferfish_backend.service" ) ]
then
    sudo cp $config_dir/pufferfish_backend.service /etc/systemd/system/
    sudo sed -i "s|python_file_absolute_path|$backend_file|g" /etc/systemd/system/pufferfish_backend.service
    sudo chmod 644 /etc/systemd/system/pufferfish_backend.service
else
    echo -e "${ERROR} The pufferfish_backend.service file doesn't exist${NC}"
    exit 1
fi

# Enabling service
sudo systemctl daemon-reload
sudo systemctl set-default pufferfish.target
sudo systemctl enable pufferfish_backend.service

echo -e "\n${SUCCESS}Backend Service setup complete\n${NC}"