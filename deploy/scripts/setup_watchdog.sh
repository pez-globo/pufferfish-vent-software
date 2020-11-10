#!/bin/bash

# Sets up watchdog to handle raspberry pi failures and crashes

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up Watchdog **********\n${NC}"

# Loading internal watchdog kernel module
sudo modprobe bcm2835_wdt || exit_script "Could not load watchdog module"
if [ 0 -eq $( grep -c "^bcm2835_wdt" /etc/modules ) ]
then
    echo -e "\nbcm2835_wdt" | sudo tee -a /etc/modules
fi

# Installing watchdog
sudo apt install watchdog -y || exit_script "Could not install watchdog"

# Adding watchdog config
if [ 1 -eq $( ls $config_dir | grep -c "watchdog_config.txt" ) ]
then
    cat $config_dir/watchdog_config.txt | sudo tee -a /etc/watchdog.conf
else
    echo -e "${ERROR} Configuration file (watchdog_config.txt) not found!${NC}"
    exit 1
fi

# Starting watchdog service
sudo systemctl start watchdog
sudo systemctl daemon-reload

echo -e "\n${SUCCESS}Watchdog setup complete\n${NC}"