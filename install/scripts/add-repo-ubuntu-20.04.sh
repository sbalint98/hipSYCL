#!/bin/bash
export DEBIAN_FRONTEND=noninteractive
apt update -y 
apt install -y wget gawk gnupg apt-utils build-essential
echo "deb http://repo.urz.uni-heidelberg.de/sycl$1/deb/ ./focal main" > /etc/apt/sources.list.d/hipsycl.list
wget -q -O - http://repo.urz.uni-heidelberg.de/sycl/hipsycl.asc | apt-key add -
apt update

