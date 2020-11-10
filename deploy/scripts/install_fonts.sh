#!/bin/bash

# Installs Roboto font

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

cd ~/

echo -e "\n${SUCCESS}********** Installing Roboto Font **********\n${NC}"

# Creating fonts folder if it doesn't exist
mkdir -p .fonts

# Downloading font
if [ $( ls .fonts/ | grep -c "Roboto" ) -ge 2 ]
then
    echo -e "${WARNING} Roboto fonts already available${NC}"
    exit
else
    wget -O roboto.zip "https://fonts.google.com/download?family=Roboto"
fi

sudo apt install unzip -y || exit_script "Could not install unzip"

# Extracting downloaded fonts zip file
if [ 1 -eq $( ls | grep -c "roboto.zip" ) ]
then
    unzip roboto.zip -d .fonts/
    rm roboto.zip
else
    echo -e "${ERROR} Fonts download failed${NC}"
    exit 1
fi

echo -e "\n${SUCCESS}Roboto fonts installation complete\n${NC}"