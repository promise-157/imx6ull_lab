file(GLOB SOURCES "src/*.cpp" "include/*.h")
add_library(service_lib STATIC ${SOURCES})
target_include_directories(service_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(service_lib PRIVATE Qt5::Core Qt5::Widgets core_utils_lib)
