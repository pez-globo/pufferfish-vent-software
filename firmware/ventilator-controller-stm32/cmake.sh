#!/bin/bash

BUILD_TARGET="$1"

if [ "$BUILD_TARGET" == "TestCatch2" ]; then
  TOOLCHAIN_ARGS=""
else
  TOOLCHAIN_ARGS="\
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake \
    -DTOOLCHAIN_PREFIX=$2 \
    -DTOOLCHAIN_BIN_DIR=$3"
fi

CMAKE_BUILD_DIR=`echo "cmake-build-$BUILD_TARGET" | awk '{print tolower($0)}'`
rm -rf $CMAKE_BUILD_DIR
mkdir $CMAKE_BUILD_DIR
cd $CMAKE_BUILD_DIR
cmake .. \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_BUILD_TYPE="$1" \
  $TOOLCHAIN_ARGS
cd ..
