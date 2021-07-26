添加WiFi驱动的绝对路径：

export WDRV_DIR= wifi的绝对路径
export HOST_PLAT=pc                    (新环境需要在platform.mak中添加)

make clean
make

// s9082、s9083驱动加载
sudo insmod s9083.ko fw=./fw/ram-fw-908x-old-r1549.bin

// 加密的9188 9189驱动加载
insmod s9188.ko fw=./fw/ram-fw-9188-old-r1576-enc-00.bin

/usr/wpa_supplicant -Dwext -iwlan0_u0 -c../wpa_supplicant.conf -B \r\n

// 测速上下电脚本
./test_pwr.sh s9083.ko 192.168.123.35 wlan0_u0 /etc/wpa_supplicant.conf
./test_pwr_stm.sh s9083.ko 192.168.123.35 wlan0_u0
