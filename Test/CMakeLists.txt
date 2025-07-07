add_subdirectory(Core)
add_subdirectory(Common)
add_subdirectory(Resource)
add_subdirectory(Platform)
add_subdirectory(Function)

add_custom_target(
    Data
    ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Test/Data
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Data ${CMAKE_BINARY_DIR}/bin/Test/Data
    COMMENT "Copying Project files from source_folder to destination_folder..."
    VERBATIM
)

add_dependencies(
    Common
    Data
)