#!/bin/bash
source /etc/profile
set -e 

DEVOPS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
SCRIPT_DIR=${SCRIPT_DIR:-../../install/scripts/}
SCRIPT_DIR_ABS=$DEVOPS_DIR/$SCRIPT_DIR
REPO_BASE_DIR=${REPO_BASE_DIR:-/data/repos/}

BUILD_HIPSYCL=${BUILD_HIPSYCL:-ON}
BUILD_CUDA=${BUILD_CUDA:-OFF}
BUILD_ROCM=${BUILD_ROCM:-OFF}
BUILD_BASE=${BUILD_BASE:-OFF}
NO_BUILD=${NO_BUILD:-OFF}

HIPSYCL_REPO_USER=${HIPSYCL_REPO_USER:-illuhad}
HIPSYCL_REPO_BRANCH=${HIPSYCL_REPO_BRANCH:-master}

LLVM_REPO_BRANCH=${LLVM_REPO_BRANCH:-release/9.x}
CONTAINER_DIR_SUFFIX=${CONTAINER_DIR_SUFFIX:-containers}
CONTAINER_DIR_NAME=${LLVM_REPO_BRANCH/release\//llvm-}
CONTAINER_DIR=$SCRIPT_DIR_ABS/${CONTAINER_DIR_NAME}-${CONTAINER_DIR_SUFFIX}

#[ "$LLVM_REPO_BRANCH" = "release/9.x" ] && CONTAINER_DIR=$SCRIPT_DIR_ABS/clang9-$CONTAINER_DIR_SUFFIX

export CONTAINER_DIR
export BUILD_HIPSYCL
export BUILD_CUDA
export BUILD_ROCM
export BUILD_BASE
export NO_BUILD
export LLVM_REPO_BRANCH
export HIPSYCL_REPO_USER
export HIPSYCL_REPO_BRANCH
#export SINGULARITY_TMPDIR=/data/sbalint/singularity_tmp/
#export HIPSYCL_GPG_KEY=B2B75080


[ "$BUILD_CUDA" = "ON" ] || [ "$BUILD_ROCM" = "ON" ] || \
[ "$BUILD_BASE" = "ON" ] && [ "$NO_BUILD" = "OFF" ] &&  \
bash $SCRIPT_DIR_ABS/rebuild-base-images.sh

[ "$BUILD_HIPSYCL" = "ON" ] && [ "$NO_BUILD" = "OFF" ] && \
bash $SCRIPT_DIR_ABS/rebuild-hipsycl-images.sh

bash $DEVOPS_DIR/create_pkgs.sh

bash $DEVOPS_DIR/create_repos.sh

# cleanup
rm -rf /data/sbalint/singularity_tmp/*
