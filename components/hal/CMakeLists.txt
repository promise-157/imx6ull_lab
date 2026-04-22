file(GLOB SOURCES "src/*.cpp" "include/*.h")
add_library(hal_lib STATIC ${SOURCES})
target_include_directories(hal_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(hal_lib PRIVATE Qt5::Core Qt5::Widgets Qt5::Multimedia core_utils_lib)
