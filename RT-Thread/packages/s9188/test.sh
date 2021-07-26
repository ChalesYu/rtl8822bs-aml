#!/bin/sh

run_wpa=1
while true
do
   insmod s9083.ko
   if [ $run_wpa -eq 1 ]
      then
      run_wpa=0
      echo "run wpa_supplicant"
      /usr/wpa_supplicant -Dwext -iwlan0_u0  -c/mnt/mtdblock2/wpa_supplicant.conf -B
   fi
   sleep 10s
   rmmod s9083
   sleep 5s
done
