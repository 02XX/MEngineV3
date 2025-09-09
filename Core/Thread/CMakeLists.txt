file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
add_library(Thread ${RESOURCE})
target_include_directories(Thread PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Thread PUBLIC unofficial::concurrentqueue::concurrentqueue)


