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
sed -i '/%setup/a cp ./llvm_aomp_versions ${SINGULARITY_ROOTFS}/llvm_aomp_versions' hipsycl-ubuntu-18.04.def
sed -i '/%setup/a cp ./llvm_aomp_versions ${SINGULARITY_ROOTFS}/llvm_aomp_versions' hipsycl-archlinux-rolling.def
sed -i '/%setup/a cp ./llvm_aomp_versions ${SINGULARITY_ROOTFS}/llvm_aomp_versions' hipsycl-centos-7.def

sed -i "/%post/a . ./llvm_aomp_versions" hipsycl-ubuntu-18.04.def
sed -i "/%post/a . ./llvm_aomp_versions" hipsycl-archlinux-rolling.def
sed -i "/%post/a . ./llvm_aomp_versions" hipsycl-centos-7.def

sed -i "s|From: *|From: $HIPSYCL_PKG_CONTAINER_DIR/|g" hipsycl-ubuntu-18.04.def
sed -i "s|From: *|From: $HIPSYCL_PKG_CONTAINER_DIR/|g" hipsycl-archlinux-rolling.def
sed -i "s|From: *|From: $HIPSYCL_PKG_CONTAINER_DIR/|g" hipsycl-centos-7.def


sudo singularity build -F $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-ubuntu-18.04.sif hipsycl-ubuntu-18.04.def
sudo singularity build -F $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-archlinux-rolling.sif hipsycl-archlinux-rolling.def
sudo singularity build -F $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-centos-7.sif hipsycl-centos-7.def
