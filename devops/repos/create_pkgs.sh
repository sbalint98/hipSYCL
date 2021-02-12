#!/bin/bash
set -e
distro=$1
if [ -z $1 ]; then
  echo "Provide the name of the distro as the first command line argument"
  exit -1
fi

source $HIPSYCL_PKG_DEVOPS_DIR/common/init.sh

HIPSYCL_PKG_DEVOPS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
HIPSYCL_PKG_SCRIPT_DIR=${HIPSYCL_PKG_SCRIPT_DIR:-../../install/scripts/}
HIPSYCL_PKG_SCRIPT_DIR_ABS=$HIPSYCL_PKG_DEVOPS_DIR/$HIPSYCL_PKG_SCRIPT_DIR

cd $HIPSYCL_PKG_SCRIPT_DIR_ABS/packaging

stage_dir=${stage_dir[$distro]}
echo "Starting Ubuntu 18.04 pkg build"
singularity exec $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-$distro bash ${packaging_script[$distro]}
echo "Moving to staging dir  $stage_dir"
mkdir -p $HIPSYCL_PKG_DEVOPS_DIR/$stage_dir
for file in `find /tmp/hipsycl-packages | grep ${find_built_pkg[$distro]}`; do
  mv $file $HIPSYCL_PKG_DEVOPS_DIR/$stage_dir/
done
#find /tmp/hipsycl-packages/  -name "*${find_built_pkg[$distro]}*" -exec mv {} $stage_dir ";"
#first="find /tmp/hipsycl-packages/" 
#last=" exec mv {} $stage_dir  \;"
#eval printf "%q" "$first $middle $last"
##eval $first $middle  $last
#eval $cmd
rm -rf /tmp/hipsycl-packages/

#stage_dir=$HIPSYCL_PKG_DEVOPS_DIR/$HIPSYCL_PKG_CENTOS_PKG_DIR
#echo "Starting Centos build"
#singularity exec $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-centos-7 sh make-centos-pkg.sh
#echo "Moving to stage dir $stage_dir"
#mkdir -p $stage_dir
#find /tmp/hipsycl-packages/ -name \*.rpm -exec mv {} $HIPSYCL_PKG_DEVOPS_DIR/$HIPSYCL_PKG_CENTOS_PKG_DIR/ \;
#rm -rf /tmp/hipsycl-packages/
#
#stage_dir=$HIPSYCL_PKG_DEVOPS_DIR/$HIPSYCL_PKG_ARCH_PKG_DIR
#echo "Starting arch build"
#singularity exec $HIPSYCL_PKG_CONTAINER_DIR/hipsycl-archlinux-rolling sh make-archlinux-pkg.sh
#echo "Moving to staging dir  $stage_dir"
#mkdir -p $stage_dir
#find /tmp/hipsycl-packages/ \( -name \*.pkg.tar.zst -o -name \*.pkg.tar.zst.sig \)  -exec mv {} $HIPSYCL_PKG_DEVOPS_DIR/$HIPSYCL_PKG_ARCH_PKG_DIR/ \;
#
##find /tmp/hipsycl-packages/ -name \*.pkg.tar.zst.sig -exec mv {} $HIPSYCL_PKG_DEVOPS_DIR/$HIPSYCL_PKG_ARCH_PKG_DIR/ \;
#
#rm -rf /tmp/hipsycl-packages/
#


