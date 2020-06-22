export HIPSYCL_INSTALL_PREFIX=${HIPSYCL_INSTALL_PREFIX:-/opt/hipSYCL}

HIPSYCL_PKG_AOMP_RELEASE=${HIPSYCL_PKG_AOMP_RELEASE:-0.7-7}
HIPSYCL_PKG_AOMP_TAG=${HIPSYCL_PKG_AOMP_TAG:-rel_${HIPSYCL_PKG_AOMP_RELEASE}}

set -e
HIPSYCL_ROCM_BUILD_DIR=${HIPSYCL_ROCM_BUILD_DIR:-$HOME/git/aomp11}

export CC=${HIPSYCL_BASE_CC:-/opt/hipSYCL/llvm/bin/clang}
export CXX=${HIPSYCL_BASE_CXX:-/opt/hipSYCL/llvm/bin/clang++}
export SUDO=${SUDO:-"disable"}
export AOMP=$HIPSYCL_INSTALL_PREFIX/rocm
export BUILD_TYPE=Release
#export NVPTXGPUS=60,61,62,70
#export AOMP_BUILD_HIPSYCL_ESSENTIAL=1
export AOMP_BUILD_HIP=1
export CUDA=${CUDA:-$HIPSYCL_INSTALL_PREFIX/cuda}
#export AOMP_BUILD_CUDA=1

if [ -d "$HIPSYCL_ROCM_BUILD_DIR" ]; then
       read -p  "The build directory already exists, do you want to use $HIPSYCL_ROCM_BUILD_DIR anyways?[y]" -n 1 -r
       echo 
       if [[ $REPLY =~ ^[Yy]$ ]]; then
              echo "Using the exisiting directory"
       else
              echo "Please specify a different directory, exiting"
              [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
       fi
else
       echo "Cloning aomp"
       git clone -b $HIPSYCL_PKG_AOMP_TAG https://github.com/ROCm-Developer-Tools/aomp $HIPSYCL_ROCM_BUILD_DIR/aomp
       cd $HIPSYCL_ROCM_BUILD_DIR/aomp/bin
       ./clone_aomp.sh
fi

cd $HIPSYCL_ROCM_BUILD_DIR/aomp/bin
COMPONENTS="select roct rocr project libdevice extras comgr vdi hipvdi"
./build_aomp.sh $COMPONENTS
