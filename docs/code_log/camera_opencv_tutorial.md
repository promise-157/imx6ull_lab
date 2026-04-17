# IMX6ULL OpenCV 与摄像头拓展移植指南

在嵌入式 Linux (IMX6ULL) 上实现摄像头的拓展，通常通过 V4L2 (Video for Linux 2) 框架获取图像，再借助 OpenCV 进行图像处理，最后在 Qt 界面上显示。

整个过程分为四大步：**环境移植**、**工程配置**、**模块化代码编写**、**硬件与程序调试**。

---

## 1. 环境移植与交叉编译 OpenCV

要在板子上运行 OpenCV，您不能直接用 `apt-get install`，必须使用与开发板一致的交叉编译器（如 Yocto SDK 或 arm-linux-gnueabihf）自行编译 OpenCV 源码。

### 1.1 准备工作
1. 在 Ubuntu 虚拟机中下载 OpenCV 源码 (如 opencv-4.5.5)。
2. 确保之前项目中用到的交叉编译环境已经 source 加载（如 `/opt/fsl-imx-x11/.../environment-setup-cortexa7hf-neon-poky-linux-gnueabi`）。

### 1.2 编译配置 (CMake)
新建一个 build 目录，执行 cmake 时，**必须开启 V4L2 支持**，关闭不需要的桌面 GUI 模块：

```bash
mkdir build && cd build
cmake -D CMAKE_SYSTEM_NAME=Linux \
      -D CMAKE_SYSTEM_PROCESSOR=arm \
      -D CMAKE_C_COMPILER=${CC} \
      -D CMAKE_CXX_COMPILER=${CXX} \
      -D WITH_QT=OFF \
      -D WITH_GTK=OFF \
      -D WITH_V4L=ON \
      -D CMAKE_INSTALL_PREFIX=~/opencv_arm_install \
      ..
make -j4
make install
```
*编译完成后，将 `~/opencv_arm_install/lib` 里的 `.so` 动态库文件拷贝到真机 IMX6ULL 的 `/usr/lib` 目录下。*

---

## 2. 引入当前 Qt 工程 (CMakeLists.txt 拓展)

在你的项目中，不需要改变现有解耦架构。只需在 `CMakeLists.txt` 中指明 OpenCV 库的位置：

```cmake
# 在 CMakeLists.txt 中增加：
set(OpenCV_DIR "~/opencv_arm_install/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

# 在 target_link_libraries 中追加 OpenCV 库
target_link_libraries(MyQtApp PRIVATE 
    Qt5::Widgets ...
    ${OpenCV_LIBS}
)
```

---

## 3. 代码模块化开发思路 (CameraPage_copy)

按照我们之前定好的 `IAppModule` 规范，您可以这样开发新的模块：

1. **创建副本文件**: `CameraPage_copy.h` 和 `CameraPage_copy.cpp`。
2. **多线程捕获**: 摄像头采集视频流是阻塞操作（一秒 30 帧），**绝对不能放在主线程**。需要另开一个 `QThread` 专门执行 `cv::VideoCapture cap(0); cap >> frame;`。
3. **格式转换**: OpenCV 的图像是 BGR 格式的 `cv::Mat`，Qt 需要的是 RGB 格式的 `QImage`。
   * 抓取帧后通过 `cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);` 转换。
   * 将 `cv::Mat` 包装为 `QImage::Format_RGB888`。
4. **信号槽投递**: 在子线程获取到 `QImage` 后，通过 Signals/Slots 发送给主线程界面的 `QLabel` 进行 `setPixmap` 显示。
5. **挂载应用**: 在 `main.cpp` 里实例化 `CameraPage_copy` 并执行 `w.registerApp(...)`。

---

## 4. 摄像头硬件与驱动调试

在写代码之前或遇到黑屏问题时，请务必先在 IMX6ULL 终端上验证硬件。

### 4.1 检查设备树与驱动加载
插上 USB 摄像头后，在开发板串口终端输入：
```bash
ls /dev/video*
```
如果能看到 `/dev/video0` 或 `/dev/video1`，说明驱动已经识别（USB 免驱 UVC 协议通常内核原生支持；若为 CSI 摄像头，则需要您在设备树 .dts 的 I2C 节点下配置 OV5640 等传感器节点）。

### 4.2 测试摄像头支持的格式
安装并使用 `v4l2-ctl` 工具查看摄像头支持的分辨率和帧率，防止 OpenCV 请求了摄像头不支持的格式导致崩溃：
```bash
v4l2-ctl -d /dev/video0 --list-formats-ext
```

### 4.3 基础排错
* **权限问题**: 开发板上运行 Qt 程序的系统用户是否对 `/dev/video0` 有读写权限？（如果不是 root，需要 `chmod 666 /dev/video0`）。
* **内存不足**: IMX6ULL 内存通常只有 256M/512M，处理 1080P 的画面极其吃力，建议在 OpenCV 中 `cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);` 强制降低分辨率。
