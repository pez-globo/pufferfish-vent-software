#!/bin/bash

echo "********** Installing pyenv **********"

sudo apt update
sudo apt install libffi-dev curl wget gcc make zlib1g-dev libsqlite3-dev -y
sudo apt install build-essential libssl-dev libbz2-dev libreadline-dev -y
sudo apt install libncurses5-dev libncursesw5-dev xz-utils libffi-dev liblzma-dev python-openssl -y
curl https://pyenv.run | bash

cd ~/pufferfish-vent-software/deploy
cat configs/pyenv_config.txt >> ~/.bashrc

pyenv="$HOME/.pyenv/bin/pyenv"

$pyenv install 3.7.7
$pyenv virtualenv 3.7.7 ventserver

cd ~/pufferfish-vent-software/backend
$pyenv local ventserver