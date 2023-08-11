#!/bin/bash
#
# Setup python-3.8+ in ubuntu

# ----------------- Setup Python ------------------

# Install python2-minimal
# sudo apt-get install -y python python3-distutils

sudo apt-get install -y python3 python3-setuptools

# Upgrade to python-3.8
# sudo apt-get install -y python3.8
# sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.6 1
# sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 2
# sudo update-alternatives --auto python3

# ----------------- Setup Pip ---------------------

sudo apt-get install -y python3-pip

# setting mirror in China
mkdir -p $HOME/.pip
cat >> $HOME/.pip/pip.conf <<END
[global]
index-url = https://pypi.mirrors.ustc.edu.cn/simple/
END

# For python3 only
# curl -sSL https://bootstrap.pypa.io/get-pip.py -o get-pip.py
# sudo python3 get-pip.py && rm -rf get-pip.py

sudo pip3 install --upgrade pip

# fix netplan
# /usr/share/netplan/netplan/configmanager.py:196 - remove CSafeLoader 
sudo python3 -m pip install -U netifaces pyaml

# install modules
sudo python3 -m pip install -U wheel

# for build packages
sudo python3 -m pip install -U mako meson 
