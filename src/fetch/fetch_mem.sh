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

#Get app's Memory(USS), first one is total USS
count=0
adb shell procrank | grep TOTAL | awk "{ print "$"2 }" >> /home/ghoo/CRAT/database/"$app_name"/mem.txt
t1=$(timestamp)
while [ $count -lt $turn ] 
do
	t2=$(timestamp)
	diff=$(expr $t2 '-' $t1)
	if [ $diff -eq $time ]
	then
		t1=$(timestamp)
		count=$(( $count+1 ))
		adb shell procrank | grep "$app_pid " | awk "{ print "$"5 }" >> /home/ghoo/CRAT/database/"$app_name"/mem.txt
	fi
done
