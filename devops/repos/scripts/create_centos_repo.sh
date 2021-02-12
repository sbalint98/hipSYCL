#!/bin/bash
#set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source $DIR/../common/init.sh

CENTOS_REPO_DIR=/data/repos/rpm/centos7
echo $CENTOS_REPO_DIR
mkdir -p $CENTOS_REPO_DIR
echo $DIR
echo $HIPSYCL_PKG_CENTOS_PKG_DIR
echo "Devops dir: $HIPSYCL_PKG_DEVOPS_DIR"
echo "HIPSYCL_PKG_REPO_STAGE_DIR: $HIPSYCL_PKG_REPO_STAGE_DIR"
cd $HIPSYCL_PKG_CENTOS_PKG_DIR
for f in *
do
	echo $f
	mv $f $CENTOS_REPO_DIR
	echo "" | setsid rpmsign --addsign $CENTOS_REPO_DIR/$f
done
cp $DIR/hipsycl.repo $CENTOS_REPO_DIR
createrepo $CENTOS_REPO_DIR
