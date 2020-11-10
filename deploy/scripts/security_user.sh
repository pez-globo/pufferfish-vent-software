#!/bin/bash

# Locks pi user and removes pi from sudoers

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up User Security **********\n${NC}"

# Lock pi and root user
sudo passwd -l pi
sudo passwd -l root

# Remove sudo permissions from pi user
sudo deluser pi sudo
sudo mv /etc/sudoers.d/010_pi-nopasswd /etc/sudoers.d/010_pi-nopasswd.

echo -e "\n${SUCCESS}User Security setup complete\n${NC}"