#!/bin/bash
distro=$1
if [ -z $1 ]; then
  echo "Provide the name of the distro as the first command line argument"
  exit -1
fi

HIPSYCL_PKG_DEVOPS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

source $HIPSYCL_PKG_DEVOPS_DIR/common/init.sh

cd $HIPSYCL_PKG_DEVOPS_DIR
SINGULARITY_BASE_DIR=${SINGULARITY_BASE_DIR:-./containers}
HIPSYCL_PKG_REPO_BASE_DIR=${HIPSYCL_PKG_REPO_BASE_DIR:-/data/repos}
HIPSYCL_PKG_SCRIPT_DIR=${HIPSYCL_PKG_SCRIPT_DIR:-$HIPSYCL_PKG_DEVOPS_DIR/scripts}
mkdir -p $HIPSYCL_PKG_REPO_BASE_DIR


singularity exec --fakeroot -B $HIPSYCL_PKG_REPO_BASE_DIR:/data/repos/ -B $HIPSYCL_PKG_DEVOPS_DIR:$HIPSYCL_PKG_DEVOPS_DIR \
     $SINGULARITY_BASE_DIR/${repo_tools_cont[$distro]} sh $HIPSYCL_PKG_SCRIPT_DIR/${repo_script[$distro]}

#singularity exec --fakeroot -B $HIPSYCL_PKG_REPO_BASE_DIR:/data/repos/ -B $HIPSYCL_PKG_DEVOPS_DIR:$HIPSYCL_PKG_DEVOPS_DIR \
#     $SINGULARITY_BASE_DIR/ubuntu-18.04.sif bash $HIPSYCL_PKG_SCRIPT_DIR/create_ubuntu_repo.sh bionic
#
#singularity exec --fakeroot -B $HIPSYCL_PKG_REPO_BASE_DIR:/data/repos/ -B $HIPSYCL_PKG_DEVOPS_DIR:$HIPSYCL_PKG_DEVOPS_DIR \
#     $SINGULARITY_BASE_DIR/ubuntu-18.04.sif bash $HIPSYCL_PKG_SCRIPT_DIR/create_ubuntu_repo.sh focal
#
#singularity exec --fakeroot -B $HIPSYCL_PKG_REPO_BASE_DIR:/data/repos/ -B $HIPSYCL_PKG_DEVOPS_DIR:$HIPSYCL_PKG_DEVOPS_DIR \
#     $SINGULARITY_BASE_DIR/arch.sif sh $HIPSYCL_PKG_SCRIPT_DIR/create_arch_repo.sh
