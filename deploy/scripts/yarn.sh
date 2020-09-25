#!/bin/bash

echo "********** Installing yarn **********"

sudo npm i -g yarn
cd ~/pufferfish-vent-software/frontend
yarn install
yarn build