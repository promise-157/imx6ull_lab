# 1. 扫描 UI 界面的 cpp
file(GLOB SOURCES "src/*.cpp"  "include/*.h")

# 2. 声明成库
add_library(gui_pages_lib STATIC ${SOURCES})

# 3. 声明暴露头文件
target_include_directories(gui_pages_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 4. 链接下级的 core_utils_lib 和 Qt 绘图库
target_link_libraries(gui_pages_lib PUBLIC 
    core_utils_lib 
    Qt5::Widgets 
    Qt5::Multimedia 
    Qt5::Network 
    Qt5::Core 
    Qt5::Gui
    Qt5::MultimediaWidgets
)