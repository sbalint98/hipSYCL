#!/bin/bash
source /etc/profile
cd $1
set -e
pwd
source ./common/init.sh
install_dir=${2:-/data/hipsycl-builds/}
repo_dir=${3:-/data/repos/}


HIPSYCL_PKG_LLVM_REPO_BRANCH=release/10.x \
HIPSYCL_PKG_LLVM_VERSION_MAJOR=11 \
HIPSYCL_PKG_LLVM_VERSION_MINOR=0 \
HIPSYCL_PKG_LLVM_VERSION_PATCH=0 \
HIPSYCL_PKG_REPO_BASE_DIR=$repo_dir \
HIPSYCL_PKG_CONTAINER_DIR=$install_dir \
HIPSYCL_PKG_BUILD_BASE=OFF \
HIPSYCL_PKG_BUILD_ROCM=OFF \
HIPSYCL_PKG_BUILD_HIPSYCL=ON \
HIPSYCL_PKG_NO_BUILD=ON \
HIPSYCL_PKG_PACKAGE=ON \
HIPSYCL_PKG_DEPLOY=ON \
HIPSYCL_PKG_TEST=OFF \
HIPSYCL_WITH_CUDA=$cuda_state \
HIPSYCL_WITH_ROCM=$rocm_state \
HIPSYCL_PKG_TYPE="-nightly" \
HIPSYCL_REPO_BRANCH=feature/backends-as-plugins-test \
HIPSYCL_REPO_USER=sbalint98 \
bash ./update_repo.sh
