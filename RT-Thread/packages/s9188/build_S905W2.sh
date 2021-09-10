export WDRV_DIR=`pwd`
export HOST_PLAT=amlogic905W2
export ARCH=arm64
export PATH=/build_dir/android11/amlogic/android11.0/build/build-tools/path/linux-x86:/build_dir/android11/amlogic/android11.0/prebuilts/clang/host/linux-x86/clang-r383902/bin:/build_dir/android11/amlogic/android11.0/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
export CROSS_COMPILE=aarch64-linux-androidkernel-
export CLANG_TRIPLE=aarch64-linux-gnu-
export KSRC=/build_dir/android11/amlogic/android11.0/out/android11-5.4/common
if [ "clean" = "$1" ];then
    make clean
	exit 0
fi
make CC=clang HOSTCC=clang LD=ld.lld NM=llvm-nm OBJCOPY=llvm-objcopy -j4


cp *.ko ~/nfs_root/ -auv
cp fw ~/nfs_root/ -auv
#cp ../wpa_config ~/nfs_root/ -auv

#cp s9083.ko ~/share/driver/s9083_gk.ko -v
