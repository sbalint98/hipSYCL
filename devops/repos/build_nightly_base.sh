#!/bin/bash
source /etc/profile
cd $1
set -e
pwd
source ./common/init.sh
install_dir=/data/sbalint/test_spack_build_fakeroot
repo_dir=/data/repos/test_spack_build

HIPSYCL_PKG_LLVM_REPO_BRANCH=release/10.x \
HIPSYCL_PKG_LLVM_VERSION_MAJOR=11 \
HIPSYCL_PKG_LLVM_VERSION_MINOR=0 \
HIPSYCL_PKG_LLVM_VERSION_PATCH=0 \
HIPSYCL_PKG_REPO_BASE_DIR=$repo_dir \
HIPSYCL_PKG_CONTAINER_DIR=$install_dir \
HIPSYCL_PKG_BUILD_BASE=ON \
HIPSYCL_PKG_BUILD_ROCM=ON \
HIPSYCL_PKG_BUILD_HIPSYCL=OFF \
HIPSYCL_PKG_NO_BUILD=OFF \
HIPSYCL_PKG_PACKAGE=ON \
HIPSYCL_PKG_DEPLOY=ON \
HIPSYCL_PKG_TEST=OFF \
HIPSYCL_PKG_TEST_PKG=OFF \
HIPSYCL_WITH_CUDA=$cuda_state \
HIPSYCL_WITH_ROCM=$rocm_state \
HIPSYCL_PKG_TYPE="-nightly" \
HIPSYCL_PKG_NAME_SUFFIX="${pkg_suffix}$HIPSYCL_PKG_TYPE" \
HIPSYCL_REPO_BRANCH=develop \
HIPSYCL_REPO_USER=illuhad \
bash ./update_repo.sh

