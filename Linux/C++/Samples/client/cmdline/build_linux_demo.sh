#!/bin/bash

# sh path/to/build_linux_demo.sh [Debug,Release] [Rebuild]

current_dir=$(cd "$(dirname "$0")" && pwd)

build_type="Release"
if [ -n "$1" ]; then
    build_type="$1"
fi

rebuild=false
if [ -n "$2" ] && [ "$2" = "Rebuild" ]; then
    rebuild=true
fi

build_dir=${current_dir}/out/$build_type

if [ "$rebuild" = true ]; then
    echo "clean build dir..."
    rm -rf $build_dir
fi
mkdir -p $build_dir

cd $build_dir && \
    cmake -DCMAKE_BUILD_TYPE=${build_type} ../../src && \
    make -j8
