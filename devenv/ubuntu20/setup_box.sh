#!/bin/bash
#

helpme()
{
  cat << USAGE
Usage: sudo $0

Installs the stuff needed to get the VirtualBox Linux into good shape to run 
our development environment.

1. This script needs to run as root.
2. The current directory must be the devenv project directory.

USAGE
}

# ---------------------------- Main ------------------------------
#
if [[ "$1" == "-?" || "$1" == "-h" || "$1" == "--help" ]] ; then
  helpme
  exit 1
fi

echo "🌞  Setup  VirtualBox linux environment..."

# Stop on first error
set -e

# Set timezone and localtime
if [ ! -n "$TZ" ]; then
  TZ=Asia/Shanghai
fi
mv /etc/localtime /etc/localtime.bak
ln -sf /usr/share/zoneinfo/$TZ /etc/localtime

# Set language
cat >> /etc/environment <<END
LANG=en_US.UTF-8
LANGUAGE=en_US:en
LC_CTYPE=C.UTF-8
END
locale-gen en_US.UTF-8

# Update fastest cache
apt-get clean all && apt-get update
apt-get upgrade -y
apt-get install -y --allow-remove-essential net-tools g++ cmake ninja-build git 
