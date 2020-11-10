#!/bin/bash

# Disables screen blanking and screen savers
# Hides mouse cursor on idle

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up window manager configuration **********\n${NC}"

sudo apt install unclutter -y || exit_script "Could not install unclutter"

# Set up autostart
mkdir -p $HOME/.config/lxsession/LXDE-pi

# Copy default system lxde-pi configuration to user configuration
if [ 0 -eq $( ls $HOME/.config/lxsession/LXDE-pi/ | grep -c "autostart" ) ]
then
    touch $HOME/.config/lxsession/LXDE-pi/autostart
    cat /etc/xdg/lxsession/LXDE-pi/autostart > $HOME/.config/lxsession/LXDE-pi/autostart
fi

# Adding screen blanking and hide mouse cursor configuration
if [ 1 -eq $( ls $config_dir | grep -c "screen_config.txt" ) ]
then
    cat $config_dir/screen_config.txt >> $HOME/.config/lxsession/LXDE-pi/autostart
else
    echo -e "${ERROR} Configuration file (screen_config.txt) not found!${NC}"
    exit 1
fi

# Disable Alt+Tab (causes buggy behavior in Chromium with touchscreen events)
mkdir -p $HOME/.config/openbox

if [ 1 -eq $( ls $config_dir | grep -c "lxde-pi-rc.xml" ) ]
then
    touch $HOME/.config/openbox/lxde-pi-rc.xml
    cat $config_dir/lxde-pi-rc.xml > $HOME/.config/openbox/lxde-pi-rc.xml
else
    echo -e "${ERROR} Configuration file (lxde-pi-rc.xml) not found!${NC}"
    exit 1
fi

echo -e "\n${SUCCESS}Window Manager configuration setup complete\n${NC}"