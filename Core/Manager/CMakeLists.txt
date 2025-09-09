file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)



add_library(Manager ${RESOURCE})
target_include_directories(Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Manager PUBLIC Asset Utils Enums)


