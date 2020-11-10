#!/bin/bash

# License Information

# Taken from https://github.com/ghollingworth/overlayfs

# Copyright 2019 Gordon Hollingworth

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0
    
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Setting up overlayfs
# Booting /boot partition in read-only mode
# Creating a overlayfs layer on top of / partition

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up overlayfs **********\n${NC}"

# Installing initramfs
sudo apt update || exit_script "Apt Update failed"
sudo apt install initramfs-tools -y || exit_script "Could not install initramfs-tools"

# Purging plymouth to disable default boot screen
sudo apt purge plymouth -y
sudo apt autoremove -y

# Disabling console logging during boot process
if [ 0 -eq $( cat /boot/cmdline.txt | grep -c "console=tty3" ) ]
then
    sudo sed -i 's/console=tty1/console=tty3/g' /boot/cmdline.txt
else
    echo -e "${WARNING} Boot screen logging is already disabled${NC}"
fi

# Adding overlay module to initramfs modules
if ! grep overlay /etc/initramfs-tools/modules > /dev/null; then
    echo overlay | sudo tee -a /etc/initramfs-tools/modules
else
    echo -e "${WARNING} Overlay already exists in initramfs modules${NC}"
fi

# Copying the overlay mount script to initramfs scripts
if [ 1 -eq $( ls $config_dir | grep -c "overlay" ) ]
then
    sudo cp $config_dir/overlay /etc/initramfs-tools/scripts
else
    echo -e "${ERROR} The overlay file doesn't exist${NC}"
    exit 1
fi

# Creating a new initramfs img
if [ 0 -eq $( ls /boot/ | grep -c "initrd7.img" ) ]
then
    sudo update-initramfs -c -k $(uname -r)
    sudo mv /boot/initrd.img-$(uname -r) /boot/initrd7.img
else
    echo -e "${WARNING} Updated initramfs already exists${NC}"
fi

# Adding new initramfs config and img name
if [ 0 -eq $( cat /boot/config.txt | grep -c "initramfs initrd7.img" ) ]
then
    sudo sed -e "s/initramfs.*//" -i /boot/config.txt
    echo initramfs initrd7.img | sudo tee -a /boot/config.txt
fi

# Creating a backup of cmdline.txt files
if [ 0 -eq $( cat /boot/cmdline.txt | grep -c "boot=overlay" ) ]
then
    sudo cp /boot/cmdline.txt /boot/cmdline.txt.orig
    sudo sed -e "s/\(.*\)/boot=overlay \1/" -i /boot/cmdline.txt
    sudo cp /boot/cmdline.txt /boot/cmdline.txt.overlay
else
    echo -e "${WARNING} Overlay boot flag already exists in cmdline.txt${NC}"
fi

# Copying overctl script for read-only to read-write switch
if [ 1 -eq $( ls $config_dir | grep -c "overctl" ) ]
then
    sudo cp $config_dir/overctl /usr/local/sbin
else
    echo -e "${ERROR} The overctl file doesn't exist${NC}"
    exit 1
fi

# Changing /boot entry to read-only in fstab
if [ 0 -eq $( cat /etc/fstab | grep -c "defaults,ro" ) ]
then
    sudo sed -e "s/\(.*\/boot.*\)defaults\(.*\)/\1defaults,ro\2/" -i /etc/fstab
fi

echo -e "\n${SUCCESS}Overlayfs setup complete\n${NC}"