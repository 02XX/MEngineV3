file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
add_library(UUID ${RESOURCE})
target_include_directories(UUID PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


