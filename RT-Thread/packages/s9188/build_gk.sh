export WDRV_DIR=`pwd`
export HOST_PLAT=goke
export CROSS_COMPILE=/build_dir/7202/GK720X_LinuxSDK_v1.0.0/opt/goke/toolchain/uClibc_gcc4.8.5/arm-gk720x-linux/bin/arm-gk720x-linux-uclibcgnueabi-
export KSRC=/build_dir/7202/GK720X_LinuxSDK_v1.0.0/linux/kernel/3.4.43
if [ "clean" = "$1" ];then
    make V=1 clean
	exit 0
fi
make -j4 V=1

cp *.ko ~/nfs_root/ -auv
cp fw ~/nfs_root/ -auv
cp ../wpa_config ~/nfs_root/ -auv

#cp s9083.ko ~/share/driver/s9083_gk.ko -v
