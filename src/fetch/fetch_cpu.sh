#!/bin/bash

#Get app's name & pid, time
app_pid=$1
app_name=$2
time=$3

#Get app's cpu usage(%)
adb shell top -d $time | grep "$app_pid " >> /home/ghoo/CRAT/database/"$app_name"/cpu.txt
