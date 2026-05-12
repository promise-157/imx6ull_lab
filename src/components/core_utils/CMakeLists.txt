file(GLOB SOURCES "src/*.cpp" "src/bus/*.cpp" "include/*.h" "include/bus/*.h")
add_library(core_utils_lib STATIC ${SOURCES})
target_include_directories(core_utils_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(core_utils_lib PUBLIC Qt5::Core asound pthread)
