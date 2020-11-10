#!/bin/bash

# Modifies eeprom configuration for changing boot order to boot from USB as default

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up Live USB Boot **********\n${NC}"

# Copying latest eeprom configuration
cp /lib/firmware/raspberrypi/bootloader/stable/pieeprom-2020-09-03.bin pieeprom.bin

# Modifying configuration to reduce USB boot timeout
if [ 1 -eq $( ls $config_dir | grep -c "bootconf.txt" ) ]
then
    rpi-eeprom-config --out pieeprom-new.bin --config $config_dir/bootconf.txt pieeprom.bin
    sudo rpi-eeprom-update -d -f ./pieeprom-new.bin
    rm pieeprom-new.bin
    rm pieeprom.bin
else
    echo -e "${WARNING}The bootconf.txt file doesn't exist${NC}"
    exit 1
fi

echo -e "\n${SUCCESS}Live USB Boot setup complete\n${NC}"