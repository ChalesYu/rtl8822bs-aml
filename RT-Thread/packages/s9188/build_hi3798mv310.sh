export WDRV_DIR=`pwd`
export HOST_PLAT=hi3798mv310
export CROSS_COMPILE=/build_dir/android9/Hi3798/patch_1_for_HiSTBAndroidV800R001C00SPC120_20200221/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-
export KSRC=/build_dir/android9/Hi3798/patch_1_for_HiSTBAndroidV800R001C00SPC120_20200221/out/target/product/Hi3798MV110/obj/KERNEL_OBJ
if [ "clean" = "$1" ];then
    make clean
	exit 0
fi
make -j4

cp *.ko ~/nfs_root/ -auv
cp fw ~/nfs_root/ -auv
#cp ../wpa_config ~/nfs_root/ -auv

#cp s9083.ko ~/share/driver/s9083_gk.ko -v
