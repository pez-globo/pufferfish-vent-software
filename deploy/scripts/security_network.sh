#!/bin/bash

# Sets up user and network security
# Disables SSH for root and pi user
# Allows web serving ports (443 & 80) and disables other ports
# Disables SSH service
# Installs fail2ban
# Disables Wifi

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up Network Security **********\n${NC}"

sudo apt install openssh-server nginx ufw fail2ban -y || exit_script "Could not install required packages"

# Deny ssh for pi user
if [ 0 -eq $( grep -c "^DenyUsers pi" /etc/ssh/sshd_config ) ]
then
    echo -e "\nDenyUsers pi" | sudo tee -a /etc/ssh/sshd_config
fi

# Deny ssh for root user
if [ 0 -eq $( grep -c "^PermitRootLogin no" /etc/ssh/sshd_config ) ]
then
    echo -e "\nPermitRootLogin no" | sudo tee -a /etc/ssh/sshd_config
fi

# Firewall configuration
sudo ufw default deny incoming
sudo ufw deny ssh
sudo ufw allow in "Nginx Full"
sudo ufw --force enable

# Disabling ssh services
sudo systemctl mask ssh
sudo systemctl mask avahi-daemon.service

# local backup of fail2ban configuration
sudo cp /etc/fail2ban/jail.conf /etc/fail2ban/jail.local

sudo systemctl daemon-reload

# Add configuration to disable wifi
if [ 0 -eq $( grep -c "^dtoverlay=disable-wifi" /boot/config.txt ) ]
then
    echo -e "\ndtoverlay=disable-wifi" | sudo tee -a /boot/config.txt
else
    echo -e "${WARNING} Wifi is already disabled${NC}"
fi

echo -e "\n${SUCCESS}Network Security setup complete\n${NC}"