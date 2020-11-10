#!/bin/bash

# Custom Pufferfish boot screen and its service

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up custom boot screen **********\n${NC}"

sudo apt update || exit_script "Apt Update failed"

# Copy splash image to home directory
cp $config_dir/splash.png ~/splash.png

# Disable logs on console
existing_command=$(cat /boot/cmdline.txt)
if [ 0 -eq $( cat /boot/cmdline.txt | grep -c "consoleblank=0 loglevel=1" ) ]
then
    echo $existing_command" consoleblank=0 loglevel=1 quiet vt.global_cursor_default=0" | sudo tee /boot/cmdline.txt
else
    echo -e "${WARNING} Logs are already disabled${NC}"
fi

sudo systemctl mask getty@tty1

# fim package to read the image buffer
sudo apt install fim -y || exit_script "Could not install fim"

# Create service file
if [ 1 -eq $( ls $config_dir | grep -c "splashscreen.service" ) ]
then
    sudo cp $config_dir/splashscreen.service /etc/systemd/system/
else
    echo -e "${ERROR} The splashscreen.service file doesn't exist${NC}"
    exit 1
fi

# Configure lightdm
if [ 1 -eq $( ls $config_dir | grep -c "lightdm.conf" ) ]
then
    sudo cp $config_dir/lightdm.conf /etc/lightdm/lightdm.conf
else
    echo -e "${ERROR} Lightdm configuration file doesn't exist${NC}"
    exit 1
fi

sudo apt update || exit_script "Apt Update failed"

# Masking plymouth service to deny any other service from starting it
sudo systemctl mask plymouth-start.service
sudo systemctl enable splashscreen

echo -e "\n${SUCCESS}Custom Bootscreen setup complete\n${NC}"