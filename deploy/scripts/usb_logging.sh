#!/bin/bash

# Modifies log locations from backend, kiosk and nginx services to syslog
# Redirects these syslogs to USB Device

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up USB Logging Device **********\n$NC"

media_boot="\nLABEL=LOGS      /media/pi/LOGS       auto    rw,nosuid,nodev,x-systemd.device-timeout=5,noauto,nofail          0       0"
fstab_file="/etc/fstab"

# Adding LOGS mount point to /etc/fstab
if [ 0 -eq $( grep -c '/media/pi/LOGS' $fstab_file ) ]; then
    echo -e $media_boot | sudo tee -a $fstab_file
fi

# Create mount file
if [ 1 -eq $( ls $config_dir | grep -c "media-pi-LOGS.mount" ) ]
then
    sudo cp $config_dir/media-pi-LOGS.mount /etc/systemd/system/
    sudo chmod 644 /etc/systemd/system/media-pi-LOGS.mount
else
    echo -e "${ERROR} The media-pi-LOGS.mount file doesn't exist$NC"
    exit 1
fi

# Enabling mount point service
sudo systemctl daemon-reload
sudo systemctl enable media-pi-LOGS.mount

# Changing nginx log location to USB
nginx_conf='/etc/nginx/nginx.conf'

if [ 0 -eq $( grep -c '/media/pi/LOGS' $nginx_conf ) ]; then
    sudo sed -i 's|/var/log/nginx/access.log|/media/pi/LOGS/nginx_access.log|g' $nginx_conf
    sudo sed -i 's|/var/log/nginx/error.log|/media/pi/LOGS/nginx_error.log|g' $nginx_conf
fi

# Adding rsyslog configuration
if [ 0 -eq $( grep -c 'pufferfish_backend' /etc/rsyslog.conf ) ]; then
    if [ 1 -eq $( ls $config_dir | grep -c "rsyslog.conf" ) ]
    then
        cat $config_dir/rsyslog.conf | sudo tee -a /etc/rsyslog.conf
    else
        echo -e "${ERROR} The rsyslog.conf file doesn't exist$NC"
        exit 1
    fi
fi

if [ 0 -eq $( ls /etc/ | grep -c "bash.bash_logout" ) ]
then
    sudo touch /etc/bash.bash_logout
fi

# Unmount USB on logout
if [ 0 -eq $( cat /etc/bash.bash_logout | grep -c "umount /media/pi/LOGS" ) ]
then
    echo -e "\nsudo umount /media/pi/LOGS" | sudo tee -a /etc/bash.bash_logout
fi

# Run Log rotate every hour
if [ 0 -eq $( ls /etc/cron.hourly/ | grep -c "logrotate" ) ]
then
    sudo mv /etc/cron.daily/logrotate /etc/cron.hourly/
fi

# Add logrotate config
if [ 1 -eq $( ls $config_dir | grep -c "pufferfish_logger" ) ]
then
    sudo cp $config_dir/pufferfish_logger /etc/logrotate.d/
    sudo chmod 644 /etc/logrotate.d/pufferfish_logger
    sudo chown root:root /etc/logrotate.d/pufferfish_logger
else
    echo -e "${ERROR} The pufferfish_logger file doesn't exist$NC"
    exit 1
fi

sudo systemctl daemon-reload

echo -e "\n${SUCCESS}USB Logging setup complete\n$NC"