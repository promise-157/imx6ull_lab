file(GLOB SOURCES "src/*.cpp" "include/*.h")
add_library(service_lib STATIC ${SOURCES})

# 1. 开启我们之前埋在 cpp 里的条件编译宏
target_compile_definitions(service_lib PUBLIC ENABLE_LOCAL_ASR)

# 2. 引入 Sherpa 和 NCNN 的头文件
target_include_directories(service_lib PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/3rdparty/ncnn/build-imx6ull/install/include/ncnn
    ${CMAKE_SOURCE_DIR}/src/3rdparty/sherpa-ncnn
    ${CMAKE_SOURCE_DIR}/src/3rdparty/sherpa-ncnn/build-imx6ull/install/include/sherpa-ncnn/c-api
)

# 3. 链接刚刚编译出来的各个核心静态库
target_link_libraries(service_lib PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/3rdparty/sherpa-ncnn/build-imx6ull/install/lib/libsherpa-ncnn-c-api.a
    ${CMAKE_SOURCE_DIR}/src/3rdparty/sherpa-ncnn/build-imx6ull/install/lib/libsherpa-ncnn-core.a
    ${CMAKE_SOURCE_DIR}/src/3rdparty/sherpa-ncnn/build-imx6ull/install/lib/libkaldi-native-fbank-core.a
    ${CMAKE_SOURCE_DIR}/src/3rdparty/sherpa-ncnn/build-imx6ull/install/lib/libkissfft-float.a
    ${CMAKE_SOURCE_DIR}/src/3rdparty/ncnn/build-imx6ull/install/lib/libncnn.a
)

target_include_directories(service_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(service_lib PRIVATE Qt5::Core Qt5::Widgets core_utils_lib)
