#!/bin/bash

cd $(dirname $0)
LIB_DIR=./../../SDK/lib/x86_64
if [ ! -f ${LIB_DIR}/libDingRTC.so ]; then
	echo "file ${LIB_DIR}/libDingRTC.so not exist, please check SDK"
	exit
fi
if [ ! -f ${LIB_DIR}/libffmpeg.so ]; then
	echo "file ${LIB_DIR}/libffmpeg.so not exist, please check SDK"
	exit
fi
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIB_DIR

rm -rf build
cmake -S . -B build
cd build && make
cp -f bin/server_pull_demo ./../
cd ..
rm -rf build

if [ -e server_pull_demo ]; then
	./server_pull_demo
else
	echo "build demo failed"
fi

echo "Run demo done !"