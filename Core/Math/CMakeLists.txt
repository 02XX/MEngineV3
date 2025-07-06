
find_package(glm CONFIG REQUIRED) 
add_library(Math INTERFACE)
target_include_directories(Math INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Math INTERFACE glm::glm)

