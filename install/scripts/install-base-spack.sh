#!/bin/bash
set -e
HIPSYCL_PKG_LLVM_VERSION_MAJOR=${HIPSYCL_PKG_LLVM_VERSION_MAJOR:-10}
HIPSYCL_PKG_LLVM_VERSION_MINOR=${HIPSYCL_PKG_LLVM_VERSION_MINOR:-0}
HIPSYCL_PKG_LLVM_VERSION_PATCH=${HIPSYCL_PKG_LLVM_VERSION_PATCH:-1}

llvm_version=$HIPSYCL_PKG_LLVM_VERSION_MAJOR.$HIPSYCL_PKG_LLVM_VERSION_MINOR.$HIPSYCL_PKG_LLVM_VERSION_PATCH
git clone https://github.com/spack/spack.git
export SPACK_ROOT=/root/spack
export PATH=$SPACK_ROOT/bin:$PATH
. $SPACK_ROOT/share/spack/setup-env.sh

sed -i 's|root: .*$|root: /opt/hipSYCL/llvm/|' spack/etc/spack/defaults/config.yaml
sed -i 's|all: .*$|all: ${PACKAGE}|' spack/etc/spack/defaults/config.yaml
sed -i 's|# build_jobs: .*$|build_jobs: 64|' spack/etc/spack/defaults/config.yaml
spack install llvm@$llvm_version cuda=True cuda_arch=70 libcxx=False 
spack load llvm
spack compiler find /opt/hipSYCL/llvm/llvm/
spack unload llvm
spack gc -y

sed -i 's|root: .*$|root: /opt/hipSYCL/boost/|' spack/etc/spack/defaults/config.yaml
spack compiler find /opt/hipSYCL/llvm/llvm/bin/
parallel --lb -N0 spack install boost%clang@$llvm_version context=True fiber=True  cxxstd=11 ::: {1..16}
spack gc -y

sed -i 's|root: .*$|root: /opt/hipSYCL/rocm/|' spack/etc/spack/defaults/config.yaml
spack compiler find /opt/hipSYCL/llvm/llvm/bin/
# in #19528 swr=none became swr=auto that seems to cause troubles when llvm not in spec
# lets use none for now until the hip pkgs get updated
parallel --lb -N0 spack install hip%clang@$llvm_version ^mesa swr=none ::: {1..16}
spack gc -y



