#!/bin/bash
#
# Shell script to install Bluetooth firmware and attach BT part of
# RTL8822BS for MGV2000 1/8
# based on the work of Staars lwfinger
# MGV2000 use UART_A for BT serial , use GPIOX_17 (0x60 = 96) for BT_EN pin

echo "(Re)start Bluetooth device ..."
sleep 1
echo
echo "Power off BT device now ..."
rmmod 8822BS_BT_EN


echo "starting BT device now ..."
sleep 1
insmod ./8822BS_BT_EN.ko
echo
TTY="/dev/ttyAML1"
systemctl stop serial-getty@$TTY


echo "Using device $TTY for Bluetooth"
modprobe rfkill
echo "Power cycle 8822BS BT-section"
#rfkill block bluetooth
sleep 2
rfkill unblock blueooth
echo "Start attaching"
rfkill unblock all
./8822b_hciattach -n -s 115200 $TTY rtk_h5 2>&1 &
