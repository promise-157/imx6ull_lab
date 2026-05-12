#!/bin/bash
SDK_PATH="/opt/fsl-imx-x11/4.1.15-2.1.0"
SYSROOT="$SDK_PATH/sysroots/cortexa7hf-neon-poky-linux-gnueabi"
TOOL_CHAIN="$SDK_PATH/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-"

export ncnn_DIR="$(pwd)/../ncnn/build-imx6ull/install/lib/cmake/ncnn"

# 1. 彻底清理缓存
rm -rf build-imx6ull
mkdir -p build-imx6ull && cd build-imx6ull

# 2. 关键：切换到 C++14，并禁用 JSON 的 C++17 特性检测
cmake .. \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=arm \
    -DCMAKE_SYSROOT=$SYSROOT \
    -DCMAKE_C_COMPILER=${TOOL_CHAIN}gcc \
    -DCMAKE_CXX_COMPILER=${TOOL_CHAIN}g++ \
    -DCMAKE_CXX_STANDARD=14 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_C_FLAGS="-mfloat-abi=hard -mfpu=neon-vfpv4 -mcpu=cortex-a7" \
    -DCMAKE_CXX_FLAGS="-mfloat-abi=hard -mfpu=neon-vfpv4 -mcpu=cortex-a7 -DJSON_HasOptional=0 -DJSON_HasVariants=0" \
    -DSHERPA_NCNN_ENABLE_PYTHON=OFF \
    -DSHERPA_NCNN_ENABLE_PORTAUDIO=OFF \
    -DSHERPA_NCNN_ENABLE_JNI=OFF \
    -DSHERPA_NCNN_ENABLE_BINARY=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX=./install

make -j$(nproc)
make install