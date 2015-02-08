#!/bin/bash
#Simple build script
cd panel
make
cd ../wallpaper-service
make
cd ../settings
make
cd ../plugins
make
cd ..


#Install and Run
if [ "$1" == "-r" ]
then
sudo make install
make
fi

#Install
if [ "$1" == "-i" ]
then
sudo make install
fi
