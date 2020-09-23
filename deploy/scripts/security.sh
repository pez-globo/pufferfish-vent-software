#!/bin/bash

echo "********** Setting up User & Network Security **********"

sudo apt install openssh-server nginx ufw fail2ban -y

# Deny ssh for pi user
echo "DenyUsers pi" | sudo tee -a /etc/ssh/sshd_config  

# Deny ssh for root user
echo "PermitRootLogin no" | sudo tee -a /etc/ssh/sshd_config  

# Firewall configuration
sudo ufw default deny incoming
sudo ufw deny ssh
sudo ufw allow in "Nginx Full"
sudo ufw enable

# Disabling ssh services
sudo systemctl mask ssh
sudo systemctl mask avahi-daemon.service

# local backup of fail2ban configuration
sudo cp /etc/fail2ban/jail.conf /etc/fail2ban/jail.local

sudo systemctl daemon-reload

# Add configuration to disable wifi
echo -e "\ndtoverlay=disable-wifi" | sudo tee -a /boot/config.txt

# Lock pi and root user
sudo passwd -l pi
sudo passwd -l root

# Remove sudo permissions from pi user
sudo deluser pi sudo
sudo mv /etc/sudoers.d/010_pi-nopasswd /etc/sudoers.d/010_pi-nopasswd.