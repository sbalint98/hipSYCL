#!/bin/bash
set -e

DEVOPS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
HIPSYCL_PKG_SCRIPT_DIR=${HIPSYCL_PKG_SCRIPT_DIR:-../../install/scripts/}
HIPSYCL_PKG_SCRIPT_DIR_ABS=$DEVOPS_DIR/$HIPSYCL_PKG_SCRIPT_DIR
source $DEVOPS_DIR/common/init.sh

cd $HIPSYCL_PKG_SCRIPT_DIR_ABS/packaging

echo "Starting Ubuntu 18.04 pkg build"
singularity exec $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-ubuntu-18.04.sif sh make-ubuntu-pkg.sh
echo "Moving to staging dir"
find /tmp/hipsycl-packages/ -name \*.deb -exec mv {} $DEVOPS_DIR/$UBUNTU_PKG_DIR/ \;
rm -rf /tmp/hipsycl-packages/

echo "Starting arch build"
singularity exec $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-archlinux-rolling.sif sh make-archlinux-pkg.sh
echo "Moving to stage dir"
find /tmp/hipsycl-packages/ -name \*.pkg.tar.xz -exec mv {} $DEVOPS_DIR/$ARCH_PKG_DIR/ \;
find /tmp/hipsycl-packages/ -name \*.pkg.tar.xz.sig -exec mv {} $DEVOPS_DIR/$ARCH_PKG_DIR/ \;
rm -rf /tmp/hipsycl-packages/

echo "Starting Centos build"
singularity exec $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-centos-7.sif sh make-centos-pkg.sh
echo "Moving to stage dir"
find /tmp/hipsycl-packages/ -name \*.rpm -exec mv {} $DEVOPS_DIR/$CENTOS_PKG_DIR/ \;
rm -rf /tmp/hipsycl-packages/
