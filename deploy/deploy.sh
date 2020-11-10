#!/bin/bash

# Getting absolute path of script
script_path=$(dirname $(realpath $0))

# Message colours
ERROR='\033[1;31mERROR:'
NC='\033[0m'

# Function to exit script on failure
function exit_script {
  echo -e "${ERROR} $1 ${NC}"
  exit 1
}

# Install required components
$script_path/install.sh "deploy" || exit_script "Development components installation failed"

# Setup Nginx web server for serving frontend
$script_path/scripts/nginx_setup.sh || exit_script "Nginx webserver setup failed"

# Setup OS Boot Screen
$script_path/scripts/boot_screen.sh || exit_script "Boot Screen setup failed"

# Setup backend autostarting service on boot
$script_path/scripts/backend_service.sh || exit_script "Backend service setup failed"

# Setup configuration to disable screen blanking and hiding cursor on idle
$script_path/scripts/configure_kiosk.sh || exit_script "UI configuration setup failed"

# Setup systemd service to start Chromium Browser in Kiosk Mode on boot
$script_path/scripts/kiosk.sh || exit_script "Kiosk service setup failed"

# Setup Logging in USB
$script_path/scripts/usb_logging.sh || exit_script "USB Logging setup failed"

# Modify eeprom to boot from USB Mass Storage by default
$script_path/scripts/live_usb.sh || exit_script "USB Mass Storage setup failed"

# Modify eeprom to setup watchdog to handle Raspberry Pi failures and crashes
$script_path/scripts/setup_watchdog.sh || exit_script "Watchdog setup failed"

# Enable Overlayfs and change /boot partition to read-only
$script_path/scripts/overlayfs.sh || exit_script "Read-only setup failed"

# Disable unnecessary background services
$script_path/scripts/disable_services.sh || exit_script "Disabling background services failed"

# Setup network security protocols
$script_path/scripts/security_network.sh || exit_script "Network security setup failed"

# Setup tampering security protocols
$script_path/scripts/security_tampering.sh || exit_script "Tampering security setup failed"

# Setup user security protocols
$script_path/scripts/security_user.sh || exit_script "User security setup failed"

echo -e "Reboot required for changes to take effect. Please restart!"
