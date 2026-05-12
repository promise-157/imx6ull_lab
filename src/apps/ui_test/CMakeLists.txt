# 1. 扫描应用程序的所有入口源文件 (比如 main.cpp)
file(GLOB SOURCES "*.cpp" "*.h")

# 2. 指定从全局 assets 提取的 qrc 资源
set(RESOURCES "${CMAKE_SOURCE_DIR}/src/assets/resources.qrc")

# 3. 声明最后编译出来的可执行程序名为 ui_test
add_executable(ui_test ${SOURCES} ${RESOURCES})

# 4. 把底层的组件库链接进来，并引入 Qt 的包
target_link_libraries(ui_test PRIVATE 
    gui_pages_lib 
    core_utils_lib
    Qt5::Widgets 
    Qt5::Core 
    Qt5::Gui
    Qt5::Network
    Qt5::Multimedia 
    Qt5::MultimediaWidgets
)

