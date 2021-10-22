export WDRV_DIR=`pwd`
export HOST_PLAT=pc
export CROSS_COMPILE=
if [ "clean" = "$1" ];then
    make clean
	exit 0
fi
make -j4
