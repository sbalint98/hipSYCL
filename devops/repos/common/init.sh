#!/bin/bash

REPO_STAGE_DIR=${REPO_STAGE_DIR:-./stage}

export ARCH_PKG_DIR=$REPO_STAGE_DIR/new_pkg_arch
export CENTOS_PKG_DIR=$REPO_STAGE_DIR/new_pkg_centos
export UBUNTU_PKG_DIR=$REPO_STAGE_DIR/new_pkg_ubuntu


export HIPSYCL_GPG_KEY=B2B75080
