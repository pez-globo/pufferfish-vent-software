#!/bin/bash

# Installs pyenv and sets up an environment named ventserver
# with python 3.7.7 for backend

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Installing pyenv **********\n${NC}"

# Getting absolute path of backend files
backend_path=$script_dir/../../backend

# Installing pyenv and required dependencies
if ! command -v pyenv &> /dev/null
then
    sudo apt update || exit_script "Apt Update failed"
    sudo apt install libffi-dev curl wget gcc make zlib1g-dev libsqlite3-dev -y || exit_script "Could not install required packages"
    sudo apt install build-essential libssl-dev libbz2-dev libreadline-dev -y || exit_script "Could not install required packages"
    sudo apt install libncurses5-dev libncursesw5-dev xz-utils libffi-dev liblzma-dev python-openssl -y || exit_script "Could not install required packages"
    curl https://pyenv.run | bash || exit_script "Could not install pyenv"
else
    echo -e "${WARNING} pyenv is already installed, skipping installation${NC}"
fi

if [ 0 -eq $( cat $HOME/.bashrc | grep -c "pyenv" ) ]
then
    if [ 1 -eq $( ls $config_dir | grep -c "pyenv_config.txt" ) ]
    then
        cat $config_dir/pyenv_config.txt >> ~/.bashrc
    else
        echo -e "${ERROR} Configuration file (pyenv_config.txt) not found!${NC}"
        exit 1
    fi
else
    echo -e "${WARNING} pyenv already added to path${NC}"
fi

pyenv="$HOME/.pyenv/bin/pyenv"

# Installing python 3.7.7
if [ 0 -eq $( $pyenv versions | grep -c "3.7.7" ) ]
then
    $pyenv install 3.7.7
else
    echo -e "${WARNING} Python 3.7.7 is already installed${NC}"
fi

if [ 0 -eq $( $pyenv versions | grep -c "ventserver" ) ]
then
    $pyenv virtualenv 3.7.7 ventserver
else
    echo -e "${WARNING} ventserver environment already exists!${NC}"
fi

cd $backend_path
$pyenv local ventserver

echo -e "\n${SUCCESS}Pyenv setup complete\n${NC}"