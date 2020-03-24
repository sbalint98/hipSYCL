export INSTALL_PREFIX=${INSTALL_PREFIX:-/opt/hipSYCL}

set -e
HIPSYCL_PKG_BUILD_DIR=$HOME/git/aomp
rm -rf $HIPSYCL_PKG_BUILD_DIR

#git clone -b hipsycl-0.8 https://github.com/illuhad/aomp $HIPSYCL_PKG_BUILD_DIR/aomp
git clone https://github.com/ROCm-Developer-Tools/aomp $HIPSYCL_PKG_BUILD_DIR/aomp
cd $HIPSYCL_PKG_BUILD_DIR/aomp
git fetch --all
git checkout rel_0.7-7
cd $HIPSYCL_PKG_BUILD_DIR/aomp/bin
export CC=${HIPSYCL_BASE_CC:-clang}
export CXX=${HIPSYCL_BASE_CXX:-clang++}
export SUDO=${SUDO:-"disable"}
export AOMP=$INSTALL_PREFIX/rocm
export HIPSYCL_PKG_BUILD_TYPE=Release
#export NVPTXGPUS=60,61,62,70
#export AOMP_HIPSYCL_PKG_BUILD_HIPSYCL_ESSENTIAL=1
export AOMP_HIPSYCL_PKG_BUILD_HIP=1
export CUDA=${CUDA:-$INSTALL_PREFIX/cuda}
#export AOMP_HIPSYCL_PKG_BUILD_CUDA=1

./clone_aomp.sh

sed -i 's/openmp pgmath flang flang_runtime//g' $HIPSYCL_PKG_BUILD_DIR/aomp/bin/build_aomp.sh
sed -i 's/exit 1//g' $HIPSYCL_PKG_BUILD_DIR/aomp/bin/build_hcc.sh
# This aomp patch to support HIP in conjunction with OpenMP breaks HIP clang printf,
# so we remove it
sed -i 's/patch -p1 < $thisdir\/hip.patch//g' $HIPSYCL_PKG_BUILD_DIR/aomp/bin/build_hip.sh

# Remove problematic -Werror compilation arguments
sed -i 's/ -Werror//g' $HIPSYCL_PKG_BUILD_DIR/aomp-extras/hostcall/lib/CMakeLists.txt
sed -i 's/ -Werror//g' $HIPSYCL_PKG_BUILD_DIR/rocr-runtime/src/CMakeLists.txt

# Remove for compatibility with glibc 2.31
sed -i 's/CHECK_SIZE_AND_OFFSET(ipc_perm, mode);//g' $HIPSYCL_PKG_BUILD_DIR/llvm-project/compiler-rt/lib/sanitizer_common/sanitizer_platform_limits_posix.cc
sed -i 's/CHECK_SIZE_AND_OFFSET(ipc_perm, mode);//g' $HIPSYCL_PKG_BUILD_DIR/hcc/llvm-project/compiler-rt/lib/sanitizer_common/sanitizer_platform_limits_posix.cpp

./build_aomp.sh

