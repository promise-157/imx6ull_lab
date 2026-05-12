# 本地离线端侧AI (Sherpa-NCNN) 交叉编译指南

本指南说明如何在 I.MX6ULL 上开启真正的离线语音识别功能。

## 1. 下载 NCNN 与 Sherpa-ncnn 源码
在开发主机（非板子）上执行：
```bash
cd ~/imx6ull_lab
mkdir 3rdparty && cd 3rdparty

# 获取 NCNN
git clone https://github.com/Tencent/ncnn.git
cd ncnn
git submodule update --init

# 获取 Sherpa-ncnn
cd ..
git clone https://github.com/k2-fsa/sherpa-ncnn.git
```

## 2. 交叉编译 NCNN
通过正点原子的交叉编译链（arm-poky-linux-gnueabi）来编译 NCNN 静态库：

```bash
cd ncnn
mkdir build_arm && cd build_arm

cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/arm-linux-gnueabihf.toolchain.cmake \
      -DCMAKE_C_COMPILER=/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc \
      -DCMAKE_CXX_COMPILER=/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++ \
      -DNCNN_VULKAN=OFF \
      -DNCNN_BUILD_EXAMPLES=OFF \
      -DNCNN_BUILD_TOOLS=OFF \
      ..

make -j4
make install
```

## 3. 交叉编译 Sherpa-ncnn

```bash
cd ../../sherpa-ncnn
mkdir build_arm && cd build_arm

# 指定上一步安装的 NCNN 路径
export ncnn_DIR=../../ncnn/build_arm/install/lib/cmake/ncnn

cmake -DCMAKE_C_COMPILER=/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc \
      -DCMAKE_CXX_COMPILER=/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++ \
      -DBUILD_SHARED_LIBS=OFF \
      ..

make -j4
```

## 4. 开启工程的本地AI宏

1. 打开 `components/service/CMakeLists.txt`
2. 添加 Sherpa 头文件路径：`target_include_directories(service_lib PRIVATE /home/promise/imx6ull_lab/3rdparty/sherpa-ncnn)`
3. 链接编译好的静态库：`ncnn.a` 和 `sherpa-ncnn-c-api.a` 到你的 `service_lib`
4. 为系统添加编译宏：`target_compile_definitions(service_lib PRIVATE ENABLE_LOCAL_ASR)`
5. 重新编译你的 `demo_app` 即可使用 `LocalAsrService` 进行断网级别的流畅语音连续识别。
