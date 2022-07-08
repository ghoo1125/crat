#!/bin/bash

#Timestamp
timestamp()  { 
	date "+%s"
}

#Get app's name & pid, time, turn 
app_pid=$1
app_name=$2
time=$3
turn=$4

#Get app's Network I/O, first one for init val
count=0
t1=$(timestamp)
while [ $count -lt $turn ] 
do
	t2=$(timestamp)
	diff=$(expr $t2 '-' $t1)
	if [ $diff -eq $time ]
	then
		t1=$(timestamp)
		count=$(( $count+1 ))
		adb shell cat /proc/$app_pid/net/dev | grep wlan0 >> /home/ghoo/CRAT/database/"$app_name"/net.txt
	fi
done
