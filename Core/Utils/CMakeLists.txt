file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(Stb REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)

add_library(Utils ${RESOURCE})
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Utils PRIVATE unofficial::shaderc::shaderc Common)
target_include_directories(Utils PRIVATE ${Stb_INCLUDE_DIR})

