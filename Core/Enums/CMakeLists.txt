file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)

add_library(Enums INTERFACE)

target_include_directories(Enums INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)



