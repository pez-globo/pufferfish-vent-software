#!/bin/bash

# Cleaning up packages
echo "Cleaning up packages..."
sudo apt-get update
sudo apt-get remove --purge triggerhappy logrotate dphys-swapfile piwiz -y
sudo apt-get autoremove --purge -y

# Disable default ssh password warning
echo "Disabling default ssh password warning..."
sudo rm /etc/xdg/lxsession/LXDE-pi/sshpwd.sh

# Disable swap and filesystem check
echo "Disabling swap and filesystem check..."
existing_command=$(cat /boot/cmdline.txt)
echo $existing_command" fastboot noswap ro" | sudo tee /boot/cmdline.txt

# Replace log manager
echo "Replacing log manager with busybox..."
sudo apt-get install busybox-syslogd -y
sudo apt-get remove --purge rsyslog -y

# Mounting tmpfs in /etc/fstab
echo "Adding tmpfs mounting to /etc/fstab..."
media_boot="/dev/sda1      /media/pi       ntfs    rw,nosuid,nodev,noauto,nofail,mode=777          0       0"
fstab_file="/etc/fstab"

if [ 0 -eq $( grep -c ',ro' $fstab_file ) ]; then
  sudo sed -i.bak "/boot/ s/defaults/defaults,ro/g" $fstab_file
  sudo sed -i "/ext4/ s/defaults/defaults,ro/g" $fstab_file
  sudo sed -i "/ext4/i$media_boot" $fstab_file

  echo "
tmpfs        /tmp            tmpfs   nosuid,nodev,mode=1777         0       0
tmpfs        /var/log        tmpfs   nosuid,nodev         0       0
tmpfs        /var/tmp        tmpfs   nosuid,nodev         0       0
tmpfs        /home/pi/.config tmpfs defaults,noatime,uid=pi,gid=pi,mode=0755 0 0
" >> $fstab_file
fi

# Moving/Linking system files to temp filesystem
echo "Linking required system files to tmpfs..."
sudo rm -rf /var/lib/dhcp /var/lib/dhcpcd5 /var/spool /etc/resolv.conf
sudo ln -s /tmp /var/lib/dhcp
sudo ln -s /tmp /var/lib/dhcpcd5
sudo ln -s /tmp /var/spool
sudo touch /tmp/dhcpcd.resolv.conf
sudo ln -s /tmp/dhcpcd.resolv.conf /etc/resolv.conf

sudo rm /var/lib/systemd/random-seed
sudo ln -s /tmp/random-seed /var/lib/systemd/random-seed

# Linking Xserver to temp
echo "Linking xserver to /tmp..."
ln -fs /tmp/.Xauthority /home/pi/.Xauthority
ln -fs /tmp/.xsession-errors /home/pi/.xsession-errors

# Modify /lib/systemd/system/systemd-random-seed.service
# and add <ExecStartPre=/bin/echo "" >/tmp/random-seed> in Service Section

random_seed_service="/lib/systemd/system/systemd-random-seed.service"
exec_start='ExecStartPre=/bin/echo "" >/tmp/random-seed'

sudo sed -i "/ExecStart/i$exec_start" $random_seed_service 

# Config to switch between read-only and read-write mode
echo "Adding switch alias for read-only and read-write mode..."
cat configs/bashrc_config.txt | sudo tee -a /etc/bash.bashrc

# Switch to read-only on reboot/poweroff
sudo touch /etc/bash.bash_logout
cat configs/bash_logout.txt | sudo tee -a /etc/bash.bash_logout

# Kernel Reboot on panic
echo "Configuring reboot on kernel panic..."
echo "\nkernel.panic = 10" | sudo tee -a /etc/sysctl.d/01-panic.conf

# Disabling daily update and upgrade services
echo "Disabling daily update and upgrade services..."
sudo systemctl disable apt-daily.service
sudo systemctl disable apt-daily.timer
sudo systemctl disable apt-daily-upgrade.service
sudo systemctl disable apt-daily-upgrade.timer

sudo systemctl daemon-reload