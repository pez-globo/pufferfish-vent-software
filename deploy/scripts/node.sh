#!/bin/bash

echo "********** Installing node **********"

cd ~/
wget https://nodejs.org/dist/v12.18.3/node-v12.18.3-linux-armv7l.tar.gz
tar -xzf node-v12.18.3-linux-armv7l.tar.gz
cd node-v12.18.3-linux-armv7l/
sudo cp -R * /usr/local/
cd ..
rm node-v12.18.3-linux-armv7l.tar.gz
rm -r node-v12.18.3-linux-armv7l/