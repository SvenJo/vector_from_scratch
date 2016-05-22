#!/bin/bash

BUILD_DIR=./build

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake .. -DCHECK_MEMORY_LEAKS=1
cmake --build .
 