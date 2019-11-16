export INSTALL_PREFIX=${INSTALL_PREFIX:-/opt/hipSYCL}

CUDA_INSTALLER_FILENAME=cuda_10.1.243_418.87.00_linux.run

set -e
cd /tmp
if [ ! -f $CUDA_INSTALLER_FILENAME ]; then
  wget https://developer.download.nvidia.com/compute/cuda/10.1/Prod/local_installers/$CUDA_INSTALLER_FILENAME
fi
sh $CUDA_INSTALLER_FILENAME --override --silent --toolkit --toolkitpath=$INSTALL_PREFIX/cuda
