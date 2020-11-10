#!/bin/bash

# Installs yarn and front-end dependencies
# Creates a build release for front-end with static files

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Installing yarn **********\n${NC}"

# Installing yarn
if ! command -v yarn &> /dev/null
then
    sudo npm i -g yarn || exit_script "Could not install yarn"
else
    echo -e "${WARNING} Yarn is already installed, skipping installation${NC}"
fi

cd $frontend_dir
yarn install || exit_script "Could not install yarn packages"
yarn build || exit_script "Could not create a release build"

echo -e "\n${SUCCESS}Yarn setup complete\n${NC}"