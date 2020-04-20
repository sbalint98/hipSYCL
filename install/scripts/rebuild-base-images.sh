#!/bin/bash
set -e
HIPSYCL_PKG_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
HIPSYCL_PKG_CONTAINER_DIR=${HIPSYCL_PKG_CONTAINER_DIR:-$HIPSYCL_PKG_SCRIPT_DIR/containers}
HIPSYCL_PKG_LLVM_REPO_BRANCH=${HIPSYCL_PKG_LLVM_REPO_BRANCH:-release/9.x}

mkdir -p /tmp/hipsycl-pkg-builder
cp $HIPSYCL_PKG_SCRIPT_DIR/*.def /tmp/hipsycl-pkg-builder
cp $HIPSYCL_PKG_SCRIPT_DIR/*.sh /tmp/hipsycl-pkg-builder
cd /tmp/hipsycl-pkg-builder

echo $HIPSYCL_PKG_CONTAINER_DIR
#  we write all relevant vars in a file which will be sourced later
set +e
env | grep HIPSYCL_PKG_LLVM > llvm_aomp_versions
env | grep HIPSYCL_PKG_AOMP >> llvm_aomp_versions
set -e
sed -i -e 's/^/export /' llvm_aomp_versions

# Workaround for exposing variable inside singularity post script
sed -i '/%setup/a cp ./llvm_aomp_versions ${SINGULARITY_ROOTFS}/llvm_aomp_versions' base-ubuntu-18.04.def
sed -i '/%setup/a cp ./llvm_aomp_versions ${SINGULARITY_ROOTFS}/llvm_aomp_versions' base-archlinux-rolling.def
sed -i '/%setup/a cp ./llvm_aomp_versions ${SINGULARITY_ROOTFS}/llvm_aomp_versions' base-centos-7.def

sed -i "/%post/a . ./llvm_aomp_versions" base-ubuntu-18.04.def
sed -i "/%post/a . ./llvm_aomp_versions" base-archlinux-rolling.def
sed -i "/%post/a . ./llvm_aomp_versions" base-centos-7.def

mkdir -p $HIPSYCL_PKG_CONTAINER_DIR

echo "Building Arch Linux image..."
sudo singularity build -F $HIPSYCL_PKG_CONTAINER_DIR/base-archlinux-rolling.sif base-archlinux-rolling.def
echo "Building Ubuntu 18.04 image..."
sudo singularity build -F $HIPSYCL_PKG_CONTAINER_DIR/base-ubuntu-18.04.sif base-ubuntu-18.04.def
echo "Building CentOS 7 image..."
sudo singularity build -F $HIPSYCL_PKG_CONTAINER_DIR/base-centos-7.sif base-centos-7.def
