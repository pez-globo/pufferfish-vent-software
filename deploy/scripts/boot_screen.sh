#!/bin/bash

echo "********** Setting up custom boot screen **********"

sudo apt-get update

# Copy splash image to home directory
cp configs/splash.png ~/splash.png

# Disable logs on console
existing_command=$(cat /boot/cmdline.txt)
echo $existing_command" consoleblank=0 loglevel=1 quiet vt.global_cursor_default=0" | sudo tee /boot/cmdline.txt

sudo systemctl mask getty@tty1

# fim package to read the image buffer
sudo apt install fim -y

# Create service file
sudo cp configs/splashscreen.service /etc/systemd/system/

# Configure lightdm
sudo cp configs/lightdm.conf /etc/lightdm/lightdm.conf

sudo apt-get update
sudo systemctl mask plymouth-start.service
sudo systemctl enable splashscreen

echo "System will reboot in 5 seconds..."
sudo reboot
