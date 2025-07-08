find_package(GTest CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(source ${SOURCES})
    get_filename_component(FILE_NAME_WE ${source} NAME_WE)
    add_executable(${FILE_NAME_WE} ${source})
    target_link_libraries(${FILE_NAME_WE} PUBLIC GTest::gtest GTest::gtest_main Platform glfw)
    add_test(NAME ${FILE_NAME_WE} COMMAND ${FILE_NAME_WE})
    message(STATUS "Added test: ${FILE_NAME_WE}")
endforeach()



