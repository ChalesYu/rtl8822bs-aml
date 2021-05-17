export WDRV_DIR=`pwd`
export HOST_PLAT=goke
export CROSS_COMPILE=arm-gk720x-linux-uclibcgnueabi-
export KSRC=/home/zenghua/GK720X_LinuxSDK_v1.0.0/linux/kernel/3.4.43
if [ "clean" = "$1" ];then
    make clean
	exit 0
fi
make -j4

cp s9083.ko ~/nfs_root/s9083_gk.ko -v
cp fw ~/nfs_root/ -auv
#cp s9083.ko ~/share/driver/s9083_gk.ko -v
