#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
CONTAINER_DIR=${CONTAINER_DIR:-$SCRIPT_DIR/containers}
LLVM_REPO_BRANCH=${LLVM_REPO_BRANCH:-release/9.x}
mkdir -p /tmp/hipsycl-pkg-builder
cp $SCRIPT_DIR/*.def /tmp/hipsycl-pkg-builder
cp $SCRIPT_DIR/*.sh /tmp/hipsycl-pkg-builder
cd /tmp/hipsycl-pkg-builder

echo $CONTAINER_DIR

# Workaround for exposing variable inside singularity post script
sed -i "/%post/a export\ LLVM_REPO_BRANCH=$LLVM_REPO_BRANCH" hipsycl-ubuntu-18.04.def
sed -i "/%post/a export\ LLVM_REPO_BRANCH=$LLVM_REPO_BRANCH" hipsycl-archlinux-rolling.def
sed -i "/%post/a export\ LLVM_REPO_BRANCH=$LLVM_REPO_BRANCH" hipsycl-centos-7.def

sed -i "s|From: *|From: $CONTAINER_DIR/|g" hipsycl-ubuntu-18.04.def
sed -i "s|From: *|From: $CONTAINER_DIR/|g" hipsycl-archlinux-rolling.def	
sed -i "s|From: *|From: $CONTAINER_DIR/|g" hipsycl-centos-7.def


sudo singularity build -F $CONTAINER_DIR/hipsycl-ubuntu-18.04.sif hipsycl-ubuntu-18.04.def
sudo singularity build -F $CONTAINER_DIR/hipsycl-archlinux-rolling.sif hipsycl-archlinux-rolling.def
sudo singularity build -F $CONTAINER_DIR/hipsycl-centos-7.sif hipsycl-centos-7.def

