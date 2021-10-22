export WDRV_DIR=`pwd`
export HOST_PLAT=tiny4412
export KSRC=/build_dir/4412/linux-3.0.86
export CROSS_COMPILE=/build_dir/4412/opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-
make -j4

cp *.ko ~/nfs_root/ -auv
cp ../wpa_config ~/nfs_root/ -auv
cp fw ~/nfs_root/ -auv
