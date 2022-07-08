#!/bin/bash

#Timestamp
timestamp()  {
        date "+%s"
}

#$1 space_num dealing with the awk print error, $2 app_name for getting pid
space_num=$1
app_name=$2
echo app_name = $app_name

echo "mkdir ~/CRAT/database/"$app_name""
mkdir /home/ghoo/CRAT/database/"$app_name"

#Get app's pid
pid_var=$(( space_num+4 ))
app_pid=$(adb shell b2g-ps | grep "$app_name" | awk "{ print "$"$pid_var }")
echo app_pid = $app_pid

#Execute fetch_cpu, fetch_mem, fetch_net
count=0
time=5
turn=24
echo sh fetch_cpu.sh
sh /home/ghoo/CRAT/fetch/fetch_cpu.sh $app_pid "$app_name" $time &
echo sh fetch_mem.sh
sh /home/ghoo/CRAT/fetch/fetch_mem.sh $app_pid "$app_name" $time $turn &
echo sh fetch_net.sh
sh /home/ghoo/CRAT/fetch/fetch_net.sh $app_pid "$app_name" $time $turn &

#Print left time
duration=$( expr $time '*' $turn )
t1=$(timestamp)
while [ $count -lt $turn ] 
do
        t2=$(timestamp)
        diff=$( expr $t2 - $t1 )
        if [ $diff -eq $time ]
        then
                t1=$(timestamp)
                count=$(( $count + 1 ))
		duration=$( expr $duration '-' $time )
		echo fetching... $duration secs left
        fi
done
echo Finished.

#kill proc top
top_pid=$(ps aux | grep "[t]op" | awk "{ print "$"2 }")
echo kill top: $top_pid
kill $top_pid
