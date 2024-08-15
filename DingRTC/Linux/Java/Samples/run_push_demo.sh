#!/bin/bash

# json-20131018.jar download url : https://repo1.maven.org/maven2/org/json/json/20131018/json-20131018.jar

rm -rf *.class

JAR_PATH=./../SDK
LIB_DIR=./../SDK/lib/x86_64

if [ ! -f ${JAR_PATH}/ding_rtc_linux_java.jar ]; then
	echo "file ${JAR_PATH}/ding_rtc_linux_java.jar not exist, please check SDK"
	exit
fi
if [ ! -f ${JAR_PATH}/json-20131018.jar ]; then
	echo "file ${JAR_PATH}/json-20131018.jar not exist, please check SDK"
	exit
fi
if [ ! -f ${LIB_DIR}/libDingRTCLinuxJava.so ]; then
	echo "file ${LIB_DIR}/libDingRTCLinuxJava.so not exist, please check SDK"
	exit
fi
if [ ! -f ${LIB_DIR}/libDingRTC.so ]; then
	echo "file ${LIB_DIR}/libDingRTC.so not exist, please check SDK"
	exit
fi
if [ ! -f ${LIB_DIR}/libffmpeg.so ]; then
	echo "file ${LIB_DIR}/libffmpeg.so not exist, please check SDK"
	exit
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIB_DIR

javac -d . -cp ${JAR_PATH}/ding_rtc_linux_java.jar:${JAR_PATH}/json-20131018.jar -encoding utf-8 LinuxServerPushDemo.java

java -cp .:${JAR_PATH}/ding_rtc_linux_java.jar:${JAR_PATH}/json-20131018.jar samples/LinuxServerPushDemo

rm -rf *.class

echo "Run demo done !"