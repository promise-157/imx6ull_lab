cmake_minimum_required(VERSION 3.10)

# --- 2. 声明工程 ---
project(MyQtApp)

# --- 3. 基础环境设置 ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



# --- 5. 开启 Qt 自动化构建 (MOC, UIC, RCC) ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- 6. 查找必备的 Qt 库 ---
find_package(Qt5 COMPONENTS Widgets Core Gui Network Multimedia MultimediaWidgets REQUIRED)


if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    message(STATUS "Using imx6ulltoolchain.cmake for ARM architecture.")
add_subdirectory(src/components/core_utils)
add_subdirectory(src/components/hal)
add_subdirectory(src/components/service)
add_subdirectory(src/components/gui_pages)
add_subdirectory(src/apps/demo)
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
    message(STATUS "Using defaulttoolchain.cmake for x86 architecture.")
add_subdirectory(src/components/gui_pages)
add_subdirectory(src/components/core_utils)
add_subdirectory(src/apps/ui_test)
endif()

# add_subdirectory(src/components/core_utils)
# add_subdirectory(src/components/hal)
# add_subdirectory(src/components/service)
# add_subdirectory(src/components/gui_pages)
# add_subdirectory(src/apps/demo)
