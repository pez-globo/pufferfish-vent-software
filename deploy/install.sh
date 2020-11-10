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

# Setup pyenv
$script_path/scripts/pyenv.sh || exit_script "Pyenv installation failed"

# Setup poetry and install required libraries
$script_path/scripts/poetry.sh || exit_script "Poetry installation failed"

# Setup pigpio dependency for backend
$script_path/scripts/pigpio.sh || exit_script "Pigpio installation failed"

# Setup Node
$script_path/scripts/node.sh || exit_script "Node installation failed"

# Install Roboto Fonts
$script_path/scripts/install_fonts.sh || exit_script "Roboto Fonts installation failed"

# Setup yarn and install required libraries
$script_path/scripts/yarn.sh || exit_script "Yarn installation failed"

if [[ "$@" != "deploy" ]]
then
    echo -n "Reboot required for changes to take effect. Do you want to reboot now? [y/N]: "
    read answer
    if echo "$answer" | grep -iq "^y"
    then
        sudo reboot
    fi
fi