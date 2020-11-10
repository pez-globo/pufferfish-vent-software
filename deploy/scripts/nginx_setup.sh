#!/bin/bash

# Installs nginx and adds front-end build files to 
# web serving directory

# Importing logging colours, absolute paths and exit function
script_dir=$(dirname $(realpath $0))
. $script_dir/helper.sh

echo -e "\n${SUCCESS}********** Setting up nginx **********\n${NC}"

sudo apt update || exit_script "Apt Update failed"
sudo apt install nginx -y || exit_script "Could not install nginx"

nginx_conf='/etc/nginx/nginx.conf'

# Adding 404 redirection to nginx configuration
if [ 0 -eq $( grep -c 'error_page 404 /;' $nginx_conf ) ]
then
    sudo sed -i '/access.log/i\\terror_page 404 /;' $nginx_conf
fi

# Copying build files to web serving directory
if [ 0 -eq $( ls $frontend_dir | grep -c "build" ) ]
then
    echo -e "${WARNING} Build files not found${NC}"
else
    sudo cp -r $frontend_dir/build/* /var/www/html/
fi

echo -e "\n${SUCCESS}Nginx setup complete\n${NC}"