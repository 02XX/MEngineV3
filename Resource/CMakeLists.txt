file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(assimp CONFIG REQUIRED)

add_library(Resource ${RESOURCE})
target_include_directories(Resource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Resource PUBLIC Core)
target_link_libraries(Resource PRIVATE assimp::assimp)
#Assets
add_custom_target(
    Assets
    ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Assets ${CMAKE_BINARY_DIR}/bin/Assets
    COMMENT "Copying assets files from source_folder to destination_folder..."
    VERBATIM
)
add_dependencies(Resource Assets)
add_custom_target(
    ProjectD
    ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Project
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Project ${CMAKE_BINARY_DIR}/bin/Project
    COMMENT "Copying Project files from source_folder to destination_folder..."
    VERBATIM
)
add_dependencies(Resource ProjectD)

#appsetting.json
add_custom_target(
    AppSetting
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_BINARY_DIR}/bin/appsettings.json
    COMMENT "Copying appsetting.json files from source_folder to destination_folder..."
    VERBATIM
)
add_dependencies(Resource AppSetting)

