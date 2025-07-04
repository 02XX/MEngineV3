file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
add_library(MThread ${RESOURCE})
target_include_directories(MThread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MThread PUBLIC unofficial::concurrentqueue::concurrentqueue)


