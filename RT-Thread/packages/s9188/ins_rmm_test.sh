#!/bin/bash

i=0 
times=50

while [ $i -le $times ]
do
let i++
echo '-----[' $i ']-----'
echo 'insmod s9083.ko'

#cat /proc/kmsg > ./log/log.txt &
insmod s9083.ko

c=$(( $RANDOM % 5 ))
 
echo '-sleep' $c's'
sleep $c

echo 'rmmod s9083.ko'
#cat /proc/kmsg > ./log/log.txt &
rmmod s9083
done

echo 'test finished'

