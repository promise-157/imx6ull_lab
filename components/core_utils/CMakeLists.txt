# 这是 components/core_utils 里的 CMakeLists.txt
# 职责：只负责把 src 下面的工具类编译成静态库

# 1. 扫描当前文件夹下 src 中的所有源文件
file(GLOB SOURCES "src/*.cpp" "include/*.h")

# 2. 声明编译目标为一个静态库 (STATIC)，名字叫 core_utils_lib
add_library(core_utils_lib STATIC ${SOURCES})

# 3. 【核心】：告诉外面使用这个库的人，去 include 文件夹里找头文件
target_include_directories(core_utils_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 4. 这个模块可能会用到 Qt 的基础类库
target_link_libraries(core_utils_lib PRIVATE Qt5::Core)