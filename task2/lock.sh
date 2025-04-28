#!/bin/bash

function cleanup() {
    tput cnorm
}

trap cleanup EXIT

tput civis

rm -f result.txt
touch file

pids_list=()

work_time=300

for i in {1..10}
do
    nohup ./lock file 2>/dev/null &
    pids_list+=($!)
done

echo ""
ps -aux | grep ./lock
echo ""

echo -ne "Прошло секунд: 0 из $work_time"
for i in $( eval echo {1..$work_time} )
do
    sleep 1
    echo -ne "\r"
    echo -ne "Прошло секунд: $i из $work_time"
done
echo -e "\rРезультат записан в result.txt\n"

for pid in ${pids_list[@]}
do
    kill -2 "$pid"
done

tput cnorm

rm -f nohup.out