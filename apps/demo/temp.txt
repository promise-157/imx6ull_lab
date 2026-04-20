cmake_minimum_required(VERSION 3.10)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# --- 1. 预设环境：必须放在 project() 之前 ---
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
set(SDK_TARGET_SYSROOT "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi")
set(CMAKE_SYSROOT ${SDK_TARGET_SYSROOT})
set(IMX6ULL_ARCH_FLAGS "-mfloat-abi=hard -mfpu=neon-vfpv4 -mcpu=cortex-a7")

set(CMAKE_C_FLAGS "${IMX6ULL_ARCH_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${IMX6ULL_ARCH_FLAGS} ${CMAKE_CXX_FLAGS}")
    
    # 这里的 CCACHE_PATH 只有在 ARM 模式下才生效
set(ENV{CCACHE_PATH} "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin")

# 告诉 CMake 在哪里找库和头文件
set(CMAKE_FIND_ROOT_PATH ${SDK_TARGET_SYSROOT})

# 情况 B: 本地编译 x86 (重点在这里！)
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")


# x86 模式下，清空干扰
    set(ENV{CCACHE_PATH} "")
    message(STATUS "x86 logic: CCACHE_PATH cleared.")

endif()
project(MyQtApp)

# --- 1. 基础环境设置 ---
set(CMAKE_CXX_STANDARD 17) # 借鉴教程，使用 C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 给 clangd 用的

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")

set(QT_HOST_PATH "/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/x86_64-pokysdk-linux/usr/bin")
set(OE_QMAKE_PATH_EXTERNAL_HOST_BINS "${QT_HOST_PATH}")

# 2.2 显式告知 Qt 各个工具的具体位置（防止它乱找）
set(QT_MOC_EXECUTABLE "${QT_HOST_PATH}/moc")
set(QT_UIC_EXECUTABLE "${QT_HOST_PATH}/uic")
set(QT_RCC_EXECUTABLE "${QT_HOST_PATH}/rcc")
# 指向交叉编译环境中的 Qt 配置路径
set(Qt5_DIR "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SYSROOT}/usr/lib/cmake/Qt5")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SYSROOT}/usr/lib/cmake")
endif()
# --- 3. Qt 自动化工具 (Qt 5 必须手动开启) ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)



# --- 4. 查找 Qt 5 模块 ---
find_package(Qt5 COMPONENTS Widgets Core Gui Network Multimedia MultimediaWidgets REQUIRED)

file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# --- 2. 搜集头文件 ---
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# --- 3. 资源文件 ---
set(RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")

# --- 4. 调试输出 (如果还是报错，解开下面这行的注释看打印) ---
# message(STATUS "找到的源文件列表: ${SOURCES}")
# --- 5. 目标定义 ---
add_executable(MyQtApp 
${SOURCES} 
${HEADERS} 
${RESOURCES}
)

# --- 6. 链接库 ---
target_link_libraries(MyQtApp PRIVATE 
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Gui
    Qt5::Network
    Qt5::Multimedia 
    Qt5::MultimediaWidgets
)

# --- 7. 包含路径 (为了补全更精准) ---
target_include_directories(MyQtApp PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)