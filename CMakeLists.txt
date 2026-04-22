cmake_minimum_required(VERSION 3.10)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# --- 1. 预设跨平台交叉编译环境：必须放在 project() 之前 ---
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    set(SDK_TARGET_SYSROOT "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi")
    set(CMAKE_SYSROOT ${SDK_TARGET_SYSROOT})
    set(IMX6ULL_ARCH_FLAGS "-mfloat-abi=hard -mfpu=neon-vfpv4 -mcpu=cortex-a7")

    set(CMAKE_C_FLAGS "${IMX6ULL_ARCH_FLAGS} ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${IMX6ULL_ARCH_FLAGS} ${CMAKE_CXX_FLAGS}")
        
    set(ENV{CCACHE_PATH} "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin")
    set(CMAKE_FIND_ROOT_PATH ${SDK_TARGET_SYSROOT})
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    set(ENV{CCACHE_PATH} "")
    message(STATUS "x86 logic: CCACHE_PATH cleared.")
endif()

# --- 2. 声明工程 ---
project(MyQtApp)

# --- 3. 基础环境设置 ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 4. Qt 交叉编译专属工具链路径 ---
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    set(QT_HOST_PATH "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin")
    set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS "${QT_HOST_PATH}")

    set(QT_MOC_EXECUTABLE "${QT_HOST_PATH}/moc")
    set(QT_UIC_EXECUTABLE "${QT_HOST_PATH}/uic")
    set(QT_RCC_EXECUTABLE "${QT_HOST_PATH}/rcc")
    
    set(Qt5_DIR "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SYSROOT}/usr/lib/cmake")
endif()

# --- 5. 开启 Qt 自动化构建 (MOC, UIC, RCC) ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- 6. 查找必备的 Qt 库 ---
find_package(Qt5 COMPONENTS Widgets Core Gui Network Multimedia MultimediaWidgets REQUIRED)

# --- 7. 下发任务给所有子目录

add_subdirectory(components/core_utils)
add_subdirectory(components/hal)
add_subdirectory(components/service)
add_subdirectory(components/gui_pages)
add_subdirectory(apps/demo)
