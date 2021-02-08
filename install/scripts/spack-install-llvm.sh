#!/bin/bash
set -e
HIPSYCL_PKG_LLVM_VERSION_MAJOR=${HIPSYCL_PKG_LLVM_VERSION_MAJOR:-11}
HIPSYCL_PKG_LLVM_VERSION_MINOR=${HIPSYCL_PKG_LLVM_VERSION_MINOR:-0}
HIPSYCL_PKG_LLVM_VERSION_PATCH=${HIPSYCL_PKG_LLVM_VERSION_PATCH:-0}

llvm_version=$HIPSYCL_PKG_LLVM_VERSION_MAJOR.$HIPSYCL_PKG_LLVM_VERSION_MINOR.$HIPSYCL_PKG_LLVM_VERSION_PATCH
if [ ! -d ./spack ]; then
  git clone https://github.com/spack/spack.git
fi
export SPACK_ROOT=/root/spack
export PATH=$SPACK_ROOT/bin:$PATH
. $SPACK_ROOT/share/spack/setup-env.sh

sed -i 's|root: .*$|root: /opt/hipSYCL/llvm/|' spack/etc/spack/defaults/config.yaml
sed -i 's|all: .*$|all: ${PACKAGE}|' spack/etc/spack/defaults/config.yaml
sed -i 's|# build_jobs: .*$|build_jobs: 64|' spack/etc/spack/defaults/config.yaml
sed -i 's|projects.append("clang-tools-extra")|#projects.append("clang-tools-extra")|' spack/var/spack/repos/builtin/packages/llvm/package.py

. $SPACK_ROOT/share/spack/setup-env.sh
spack install llvm@11.0.0 cuda=False libcxx=False polly=False lldb=False lld=False internal_unwind=False gold=False target=x86_64 build_type=MinSizeRel -flang 
#spack install llvm@$llvm_version cuda=False libcxx=False target=x86_64
spack load llvm
spack compiler find /opt/hipSYCL/llvm/llvm/
spack unload llvm

