#!/bin/bash
set -e
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
CONTAINER_DIR=${CONTAINER_DIR:-$SCRIPT_DIR/containers}
mkdir -p /tmp/hipsycl-pkg-builder
cp $SCRIPT_DIR/*.def /tmp/hipsycl-pkg-builder
cp $SCRIPT_DIR/*.sh /tmp/hipsycl-pkg-builder
cd /tmp/hipsycl-pkg-builder

echo $CONTAINER_DIR
# Workaround for exposing variable inside singularity post script
if [ "$LLVM_9_BUILD" == "ON" ]
then
	sed -i '/%post/a export\ LLVM_9_BUILD=ON' base-ubuntu-18.04.def
	sed -i '/%post/a export\ LLVM_9_BUILD=ON' base-archlinux-rolling.def
	sed -i '/%post/a export\ LLVM_9_BUILD=ON' base-centos-7.def
fi

mkdir -p $CONTAINER_DIR

echo "Building Arch Linux image..."
sudo singularity build -F $CONTAINER_DIR/base-archlinux-rolling.sif base-archlinux-rolling.def
echo "Building Ubuntu 18.04 image..."
sudo singularity build -F $CONTAINER_DIR/base-ubuntu-18.04.sif base-ubuntu-18.04.def
echo "Building CentOS 7 image..."
sudo singularity build -F $CONTAINER_DIR/base-centos-7.sif base-centos-7.def
