#!/bin/bash
SDK_PATH="/opt/fsl-imx-x11/4.1.15-2.1.0"
SYSROOT="$SDK_PATH/sysroots/cortexa7hf-neon-poky-linux-gnueabi"
TOOLCHAIN="$SDK_PATH/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-"

mkdir -p build-imx6ull && cd build-imx6ull

cmake .. \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=arm \
    -DCMAKE_SYSROOT=$SYSROOT \
    -DCMAKE_C_COMPILER=${TOOLCHAIN}gcc \
    -DCMAKE_CXX_COMPILER=${TOOLCHAIN}g++ \
    -DCMAKE_C_FLAGS="-mfloat-abi=hard -mfpu=neon-vfpv4 -mcpu=cortex-a7" \
    -DCMAKE_CXX_FLAGS="-mfloat-abi=hard -mfpu=neon-vfpv4 -mcpu=cortex-a7" \
    -DNCNN_VULKAN=OFF \
    -DNCNN_BUILD_EXAMPLES=OFF \
    -DCMAKE_INSTALL_PREFIX=./install

make -j$(nproc)
make install