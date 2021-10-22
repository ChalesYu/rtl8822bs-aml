#!/bin/sh
DRIVER="$1" #驱动名
IPERF_IP="$2" #iperf对端打流IP
IF_NAME="$3"  #网卡接口
WPA_CONF="$4" #wpa运行的配置文件
DEV_IP="192.168.31.66" #配置的设备IP
GATE_WAY="192.168.31.1" #路由IP
PING_NUM="6" #ping 的次数设置
if [ 4 != $# ];then
	echo "Usage: $0 <driver> <iperf_ip> <interface> <wpa_supplicant.conf>"
	exit 0
fi

i=0
while :
do
	clear
	let "i++"
	echo "----${i} begin-----"
	is_insmod=`lsmod | grep s9083`
	echo "is_insmod:"${is_insmod}
	if [ 0 != ${#is_insmod} ];then
		echo "rmmod ${DRIVER} 1"
		rmmod s9083
	fi
	insmod ${DRIVER}
	if [ 0 != $? ];then
		continue
	fi
	
	is_wpa_ok=`ps |grep wpa | wc | awk '{print $1}'`
	if [ "1" = ${is_wpa_ok} ];then
		wpa_supplicant -Dwext -c${WPA_CONF}  -i${IF_NAME} &
	fi
	while :
	do
		echo "waiting for link...."
		is_link_ok=$(iwconfig ${IF_NAME} | grep "ESSID")
		if [ 0 != ${#is_link_ok} ];then
			echo "${is_link_ok}"
			break
		fi
		sleep 1
	done
	
	ifconfig ${IF_NAME} ${DEV_IP}
	
	echo "ping ${PING_NUM}"
	ping ${GATE_WAY} -c ${PING_NUM}
	
	echo "rmmod ${DRIVER} last"
	rmmod s9083
	sleep 1
done
