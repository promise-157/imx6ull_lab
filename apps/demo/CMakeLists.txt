# 1. 扫描应用程序的所有入口源文件 (比如 main.cpp)
file(GLOB SOURCES "*.cpp" "*.h")

# 2. 指定从全局 assets 提取的 qrc 资源
set(RESOURCES "${CMAKE_SOURCE_DIR}/assets/resources.qrc")

# 3. 声明最后编译出来的可执行程序名为 demo_app
add_executable(demo_app ${SOURCES} ${RESOURCES})

# 4. 把底层的组件库链接进来，并引入 Qt 的包
target_link_libraries(demo_app PRIVATE 
    gui_pages_lib 
    core_utils_lib
    service_lib
    hal_lib
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Gui
    Qt5::Network
    Qt5::Multimedia 
    Qt5::MultimediaWidgets
)

# 5. 【极其核心：复制配置】告诉 CMake 在编译这个 app 结束后， 
# 把它同目录下的 config.json 复制粘贴到输出文件夹里去！
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/config.json" 
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")