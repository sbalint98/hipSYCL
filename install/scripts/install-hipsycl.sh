#!/bin/bash
LLVM_REPO_BRANCH=${LLVM_REPO_BRANCH:-release/9.x}
export INSTALL_PREFIX=${INSTALL_PREFIX:-/opt/hipSYCL}

set -e
BUILD_DIR=/tmp/hipSYCL-installer

HIPSYCL_REPO_USER=${HIPSYCL_REPO_USER:-illuhad}
HIPSYCL_REPO_BRANCH=${HIPSYCL_REPO_BRANCH:-master}
HIPSYCL_WITH_CUDA=${HIPSYCL_WITH_CUDA:-ON}
HIPSYCL_WITH_ROCM=${HIPSYCL_WITH_ROCM:-ON}

rm -rf "$BUILD_DIR"
mkdir -p $BUILD_DIR

git clone --recurse-submodules -b $HIPSYCL_REPO_BRANCH https://github.com/$HIPSYCL_REPO_USER/hipSYCL $BUILD_DIR
mkdir -p $BUILD_DIR/build
cd $BUILD_DIR/build

case $LLVM_REPO_BRANCH in
	release/9.x)
		LLVM_INCLUDE_PATH=$INSTALL_PREFIX/llvm/lib/clang/9.0.1/include 
		;;
	release/10.x)
		LLVM_INCLUDE_PATH=$INSTALL_PREFIX/llvm/lib/clang/10.0.0/include 
		;;
	*)
		echo "LLVM version $LLVM_REPO_BRANCH is not supported"
		exit -1
		;;
esac

cmake \
-DCMAKE_C_COMPILER=$INSTALL_PREFIX/llvm/bin/clang \
-DCMAKE_CXX_COMPILER=$INSTALL_PREFIX/llvm/bin/clang++ \
-DWITH_CPU_BACKEND=ON \
-DWITH_CUDA_BACKEND=$HIPSYCL_WITH_CUDA \
-DWITH_ROCM_BACKEND=$HIPSYCL_WITH_ROCM \
-DLLVM_DIR=$INSTALL_PREFIX/llvm/lib/cmake/llvm \
-DROCM_PATH=$INSTALL_PREFIX/rocm \
-DCUDA_TOOLKIT_ROOT_DIR=$INSTALL_PREFIX/cuda \
-DCLANG_EXECUTABLE_PATH=$INSTALL_PREFIX/llvm/bin/clang++ \
-DCLANG_INCLUDE_PATH=$LLVM_INCLUDE_PATH \
-DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
-DROCM_LINK_LINE='-rpath $HIPSYCL_ROCM_LIB_PATH -rpath $HIPSYCL_ROCM_PATH/hsa/lib -L$HIPSYCL_ROCM_LIB_PATH -lhip_hcc -lamd_comgr -lamd_hostcall -lhsa-runtime64 -latmi_runtime -rpath $HIPSYCL_ROCM_PATH/hcc/lib -L$HIPSYCL_ROCM_PATH/hcc/lib -lmcwamp -lhc_am' \
-DDISABLE_LLVM_VERSION_CHECK=ON \
..

make install
