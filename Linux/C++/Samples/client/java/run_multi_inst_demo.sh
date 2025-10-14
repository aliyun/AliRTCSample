#!/bin/bash
cd $(dirname $0)
manual=1
if [[ -n $1 ]]; then
    manual=0
fi
echo "manual mode : ${manual}"

pipe=/tmp/linux_java_pipe
if [ $manual == 1 ]; then
    if [ -p $pipe ]; then
        rm -f $pipe
    fi
    mkfifo $pipe
fi
rm -rf demo
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libs/x86_64/Release
rtc_jar=./libs/ding_rtc_linux_java.jar
json_jar=./libs/json-20131018.jar
javac -d . -cp $rtc_jar:$json_jar -encoding utf-8 *.java
echo 'build demo'
if [ $manual == 0 ]; then
    java -cp .:$rtc_jar:$json_jar demo/MultiInstDemo
fi
if [ $manual == 1 ]; then
    nohup java -cp .:$rtc_jar:$json_jar demo/MultiInstDemo $pipe > command_log.log 2>&1 &
    desc="Command list:\n\
     1 : join\n\
     2 : leave\n\
     3 : pub audio\n\
     4 : unpub audio\n\
     5 : pub video\n\
     6 : unpub video\n\
     7 : rtm send\n\
     8 : video bitrate ex: 8-500\n\
     9 : video fps ex: 9-20\n\
     10 : video dimen ex: 10-1280-720\n\
     0 : exit"
    echo -e $desc
    while [[ true ]]; do
        read -p "input command: " command
        echo $command > $pipe
        if [[ $command == "0" ]]; then
            break
        fi
    done
    rm -f $pipe
fi
rm -rf demo
echo "Test done, bye ~!!!"
