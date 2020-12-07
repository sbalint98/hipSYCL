#!/bin/bash
HIPSYCL_PKG_LLVM_VERSION_MAJOR=${HIPSYCL_PKG_LLVM_VERSION_MAJOR:-10}
HIPSYCL_PKG_LLVM_VERSION_MINOR=${HIPSYCL_PKG_LLVM_VERSION_MINOR:-0}
HIPSYCL_PKG_LLVM_VERSION_PATCH=${HIPSYCL_PKG_LLVM_VERSION_PATCH:-1}
HIPSYCL_PKG_LLVM_REPO_BRANCH=${HIPSYCL_PKG_LLVM_REPO_BRANCH:-release/${HIPSYCL_PKG_LLVM_VERSION_MAJOR}.x}

export HIPSYCL_INSTALL_PREFIX=${HIPSYCL_INSTALL_PREFIX:-/opt/hipSYCL/ }

set -e
HIPSYCL_BUILD_DIR=${HIPSYCL_BUILD_DIR:-/tmp/hipsycl-installer-sbalint}
HIPSYCL_REPO_USER=${HIPSYCL_REPO_USER:-sbalint98}
HIPSYCL_REPO_BRANCH=${HIPSYCL_REPO_BRANCH:-expose_link_comp_args_boost}
HIPSYCL_WITH_CUDA=${HIPSYCL_WITH_CUDA:-ON}
HIPSYCL_WITH_ROCM=${HIPSYCL_WITH_ROCM:-ON}

LLVM_INCLUDE_PATH=$HIPSYCL_INSTALL_PREFIX/llvm/llvm/lib/clang/${HIPSYCL_PKG_LLVM_VERSION_MAJOR}.\
${HIPSYCL_PKG_LLVM_VERSION_MINOR}.\
${HIPSYCL_PKG_LLVM_VERSION_PATCH}/include
if [ -d "$HIPSYCL_BUILD_DIR" ]; then
       read -p  "The build directory already exists, do you want to use $HIPSYCL_BUILD_DIR anyways?[y]" -n 1 -r
       echo 
       if [[ ! $REPLY =~ ^[Yy]$ ]]; then
              echo "Please specify a different directory, exiting"
              [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
       else
              echo "Using the exisiting directory"
       fi
else
echo "Cloning hipSYCL"
git clone --recurse-submodules -b $HIPSYCL_REPO_BRANCH https://github.com/$HIPSYCL_REPO_USER/hipSYCL $HIPSYCL_BUILD_DIR

fi

mkdir -p $HIPSYCL_BUILD_DIR/build
cd $HIPSYCL_BUILD_DIR/build

# We need the llvm module to be loaded in order to be able to find the openmp rt
export SPACK_ROOT=/root/spack
export PATH=$SPACK_ROOT/bin:$PATH
. $SPACK_ROOT/share/spack/setup-env.sh

sed -i 's|root: .*$|root: /opt/hipSYCL/llvm/|' $SPACK_ROOT/etc/spack/defaults/config.yaml
spack load llvm
rocm_path=/opt/hipSYCL/rocm/
cmake \
-DCMAKE_C_COMPILER=/opt/hipSYCL/llvm/llvm/bin/clang \
-DCMAKE_CXX_COMPILER=/opt/hipSYCL/llvm/llvm/bin/clang++  \
-DCMAKE_CXX_FLAGS=-D__APPLE__ \
-DWITH_CPU_BACKEND=ON \
-DWITH_CUDA_BACKEND=$HIPSYCL_WITH_CUDA \
-DWITH_ROCM_BACKEND=$HIPSYCL_WITH_ROCM \
-DLLVM_DIR=/opt/hipSYCL/llvm/llvm/ \
-DROCM_PATH=/opt/hipSYCL/rocm \
-DBOOST_PATH=/opt/hipSYCL/boost/boost \
-DCUDA_TOOLKIT_ROOT_DIR=/opt/hipSYCL/cuda \
-DCLANG_EXECUTABLE_PATH=/opt/hipSYCL/llvm/llvm/bin/clang++ \
-DCLANG_INCLUDE_PATH=$LLVM_INCLUDE_PATH \
-DCMAKE_INSTALL_PREFIX=$HIPSYCL_INSTALL_PREFIX \
-DCMAKE_PREFIX_PATH="$rocm_path/comgr/lib/cmake;$rocm_path/rocm-device-libs/lib/cmake;$rocm_path/hsa-rocr-dev/lib/cmake;$rocm_path/hsa-rocr-dev/;$rocm_path/hip/lib/cmake" \
..

make -j 16 install
