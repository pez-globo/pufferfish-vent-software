#!/bin/bash

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Installing pigpio **********\n${NC}"

sudo apt update || exit_script "Apt Update failed"
sudo apt install pigpio -y || exit_script "Could not install pigpio"

sudo systemctl enable pigpiod
sudo systemctl daemon-reload

echo -e "\n${SUCCESS}Pigpio setup complete\n${NC}"