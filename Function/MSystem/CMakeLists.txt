file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(EnTT CONFIG REQUIRED)
add_library(MSystem ${RESOURCE})
target_include_directories(MSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MSystem PUBLIC EnTT::EnTT MResource MComponent Math Core)




