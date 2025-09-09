file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(assimp CONFIG REQUIRED)

add_library(MResource ${RESOURCE})
target_include_directories(MResource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MResource PUBLIC Core)
target_link_libraries(MResource PRIVATE assimp::assimp)
#Engine
# add_custom_target(
#     Engine
#     ALL
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Assets
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Engine ${CMAKE_BINARY_DIR}/bin/Engine
#     COMMENT "Copying assets files from source_folder to destination_folder..."
#     VERBATIM
# )
# add_dependencies(MResource Engine)
# add_custom_target(
#     ProjectD
#     ALL
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Project
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Project ${CMAKE_BINARY_DIR}/bin/Project
#     COMMENT "Copying Project files from source_folder to destination_folder..."
#     VERBATIM
# )
# add_dependencies(MResource ProjectD)

#appsetting.json
add_custom_target(
    AppSetting
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_BINARY_DIR}/bin/appsettings.json
    COMMENT "Copying appsetting.json files from source_folder to destination_folder..."
    VERBATIM
)
add_dependencies(MResource AppSetting)

