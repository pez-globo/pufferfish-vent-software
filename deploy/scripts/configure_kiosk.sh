#!/bin/bash

echo "Installing unclutter..."
sudo apt install unclutter -y

script_dir=$(pwd)

cd ~/

# Set up autostart

mkdir -p .config/lxsession/LXDE-pi
touch .config/lxsession/LXDE-pi/autostart

cat /etc/xdg/lxsession/LXDE-pi/autostart > .config/lxsession/LXDE-pi/autostart
cat $script_dir/configs/screen_config.txt >> .config/lxsession/LXDE-pi/autostart

# Disable Alt+Tab (causes buggy behavior in Chromium with touchscreen events)

mkdir -p .config/openbox
touch .config/openbox/lxde-rc.xml
cat $script_dir/configs/lxde-pi-rc.xml > .config/openbox/lxde-pi-rc.xml
