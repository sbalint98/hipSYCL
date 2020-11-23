#!/bin/bash
set -e
HIPSYCL_PKG_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
HIPSYCL_PKG_CONTAINER_DIR=${HIPSYCL_PKG_CONTAINER_DIR:-$HIPSYCL_PKG_SCRIPT_DIR/containers}
HIPSYCL_PKG_LLVM_REPO_BRANCH=${HIPSYCL_PKG_LLVM_REPO_BRANCH:-release/9.x}
echo $HIPSYCL_PKG_CONTAINER_DIR
HIPSYCL_PKG_BUILD_CUDA=${HIPSYCL_PKG_BUILD_CUDA:-ON}
HIPSYCL_PKG_BUILD_ROCM=${HIPSYCL_PKG_BUILD_ROCM:-ON}
HIPSYCL_PKG_BUILD_BASE=${HIPSYCL_PKG_BUILD_BASE:-ON}

SINGULARITYENV_HIPSYCL_PKG_BUILD_CUDA=$HIPSYCL_PKG_BUILD_CUDA
SINGULARITYENV_HIPSYCL_PKG_BUILD_ROCM=$HIPSYCL_PKG_BUILD_ROCM
SINGULARITYENV_HIPSYCL_PKG_BUILD_BASE=$HIPSYCL_PKG_BUILD_BASE
SINGULARITYENV_HIPSYCL_PKG_LLVM_REPO_BRANCH=$HIPSYCL_PKG_LLVM_REPO_BRANCH
SINGULARITYENV_HIPSYCL_PKG_LLVM_VERSION_MAJOR=$HIPSYCL_PKG_LLVM_VERSION_MAJOR
SINGULARITYENV_HIPSYCL_PKG_LLVM_VERSION_MINOR=$HIPSYCL_PKG_LLVM_VERSION_MINOR
SINGULARITYENV_HIPSYCL_PKG_LLVM_VERSION_PATCH=$HIPSYCL_PKG_LLVM_VERSION_PATCH
SINGULARITYENV_HIPSYCL_PKG_AOMP_RELEASE=$HIPSYCL_PKG_AOMP_RELEASE
SINGULARITYENV_HIPSYCL_PKG_AOMP_TAG=$HIPSYCL_PKG_AOMP_TAG

export SINGULARITYENV_HIPSYCL_PKG_BUILD_CUDA
export SINGULARITYENV_HIPSYCL_PKG_BUILD_ROCM
export SINGULARITYENV_HIPSYCL_PKG_BUILD_BASE
export SINGULARITYENV_HIPSYCL_PKG_LLVM_REPO_BRANCH 
export SINGULARITYENV_HIPSYCL_PKG_LLVM_VERSION_MAJOR
export SINGULARITYENV_HIPSYCL_PKG_LLVM_VERSION_MINOR 
export SINGULARITYENV_HIPSYCL_PKG_LLVM_VERSION_PATCH 
export SINGULARITYENV_HIPSYCL_PKG_AOMP_RELEASE
export SINGULARITYENV_HIPSYCL_PKG_AOMP_TAG


echo $HIPSYCL_PKG_CONTAINER_DIR
cd $HIPSYCL_PKG_SCRIPT_DIR
mkdir -p $HIPSYCL_PKG_CONTAINER_DIR
supported_distros=("ubuntu-18.04" "archlinux-rolling" "centos-7")
for distro in "${supported_distros[@]}"
do
	echo "Building $distro image... with base pkgs"
	sudo -E singularity build --sandbox -F $HIPSYCL_PKG_CONTAINER_DIR/base-$distro.sif base-$distro.def
	echo "Building $distro hipSYCL base via spack"
	sudo -E singularity exec --writable --no-home $HIPSYCL_PKG_CONTAINER_DIR/base-$distro.sif bash /install-base-spack.sh
done


#echo "Building Ubuntu 18.04 image..."
#sudo -E singularity build --sandbox -F $HIPSYCL_PKG_CONTAINER_DIR/base-ubuntu-18.04.sif base-ubuntu-18.04.def
#echo "Building CentOS 7 image..."
#sudo -E singularity build --sandbox -F $HIPSYCL_PKG_CONTAINER_DIR/base-centos-7.sif base-centos-7.def
