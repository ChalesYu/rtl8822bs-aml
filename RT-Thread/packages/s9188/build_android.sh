#!/bin/bash
export WDRV_DIR=`pwd`
export HOST_PLAT=tiny4412
export KSRC=~/sdb/linux-3.0.86
make
cp s9083.ko ~/nfs_root/s9083_android.ko -v