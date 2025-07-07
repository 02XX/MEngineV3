

add_library(MComponent INTERFACE)
target_include_directories(MComponent INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(MComponent INTERFACE MAsset)

