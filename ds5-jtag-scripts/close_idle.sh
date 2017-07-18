#!/system/bin/sh

for i in 0 1 2 3; do
    for j in 0 1  ; do
        echo 1 > /sys/devices/system/cpu/cpu$i/cpuidle/state$j/disable;
    done;
done
for i in 4 5 6 7; do
    for j in 0 1 2 3  ; do
        echo 1 > /sys/devices/system/cpu/cpu$i/cpuidle/state$j/disable;
    done;
done