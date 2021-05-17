添加WiFi驱动的绝对路径：

export WDRV_DIR= wifi的绝对路径
export HOST_PLAT=pc                    (新环境需要在platform.mak中添加)

make clean
make

// s9082、s9083驱动加载
sudo insmod s9083.ko fw=./fw/ram-fw-908x-old-r1548.bin

// 加密的9188 9189驱动加载
//insmod s9083.ko fw_usb=./fw/ram-fw-9188U-new-r1485-enc.bin  fw_sdio=./fw/ram-fw-9188S-new-r1485-enc.bin
//insmod s9083.ko fw_usb=./fw/ram-fw-9188U-old-r1485-enc.bin  fw_sdio=./fw/ram-fw-9188S-old-r1485-enc.bin
insmod s9188_fpga.ko fw=./fw/ram-fw-9188-old-r1549-enc.bin
// 测速上下电脚本
./test_pwr.sh s9083.ko 192.168.123.35 wlan0_u0 /etc/wpa_supplicant.conf
./test_pwr_stm.sh s9083.ko 192.168.123.35 wlan0_u0
