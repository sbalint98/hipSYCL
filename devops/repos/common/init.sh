#!/bin/bash

HIPSYCL_PKG_REPO_STAGE_DIR=${HIPSYCL_PKG_REPO_STAGE_DIR:-./stage}

export HIPSYCL_PKG_ARCH_PKG_DIR=$HIPSYCL_PKG_REPO_STAGE_DIR/new_pkg_arch
export HIPSYCL_PKG_CENTOS_PKG_DIR=$HIPSYCL_PKG_REPO_STAGE_DIR/new_pkg_centos
export HIPSYCL_PKG_UBUNTU_PKG_DIR=$HIPSYCL_PKG_REPO_STAGE_DIR/new_pkg_ubuntu


export HIPSYCL_GPG_KEY=B2B75080

declare -A install_cmd=( ["archlinux-rolling"]="pacman -Sy --noconfirm hipSYCL" \
                         ["centos-7"]="yum -y install hipSYCL" \
                         ["centos-8"]="yum -y install hipSYCL" \
                         ["ubuntu-18.04"]="apt -y install hipsycl" \
                         ["ubuntu-20.04"]="apt -y install hipsycl"
                       )

declare -A cleanup_cmd=( ["archlinux-rolling"]="pacman -Rsn --noconfirm hipSYCL" \
                         ["centos-7"]="yum -y remove hipSYCL" \
                         ["centos-8"]="yum -y remove hipSYCL" \
                         ["ubuntu-18.04"]="apt -y remove hipsycl" \
                         ["ubuntu-20.04"]="apt -y remove hipsycl"
                       )

declare -A cleanup_dep=( ["archlinux-rolling"]='pacman -Rsn --noconfirm $(pacman -Qdtq)' \
                         ["centos-7"]="package-cleanup -y --leaves" \
                         ["centos-8"]="package-cleanup -y --leaves" \
                         ["ubuntu-18.04"]="apt -y autoremove" \
                         ["ubuntu-20.04"]="apt -y autoremove"
                       )


declare -A image_base=( ["archlinux-rolling"]="docker://archlinux/base" \
                         ["centos-7"]="docker://centos:centos7" \
                         ["centos-8"]="docker://centos:centos8" \
                         ["ubuntu-18.04"]="docker://ubuntu:18.04" \
                         ["ubuntu-20.04"]="docker://ubuntu:20.04" 
                      )

#Order of on-off: ROCM, CUDA
declare  -A pkg_suffix=( ["ONON"]="" ["OFFOFF"]="-omp" ["OFFON"]="-omp-cuda" \
                         ["ONOFF"]="-omp-rocm")

distros=( "centos-7" "archlinux-rolling" "ubuntu-18.04" "ubuntu-20.04")
