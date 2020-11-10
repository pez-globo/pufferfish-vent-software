#!/bin/bash

# Installs poetry and the backend package dependencies

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Installing poetry **********\n${NC}"

poetry="$HOME/.poetry/bin/poetry"
ventserver_env="$HOME/.pyenv/versions/3.7.7/envs/ventserver/bin/python"

# Installing poetry
if ! command -v poetry &> /dev/null
then
    curl -sSL https://raw.githubusercontent.com/python-poetry/poetry/master/get-poetry.py | python3 || exit_script "Could not install poetry"
else
    echo -e "${WARNING} Poetry is already installed, skipping installation${NC}"
fi

# Getting absolute path of backend files
backend_path=$script_dir/../../backend

# Installing backend package dependencies
cd $backend_path
$poetry config virtualenvs.create false
$ventserver_env $poetry install

echo -e "\n${SUCCESS}Poetry setup complete\n${NC}"