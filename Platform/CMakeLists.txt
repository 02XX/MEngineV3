file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
add_library(Platform ${RESOURCE})
target_include_directories(Platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Platform PUBLIC Vulkan::Vulkan Common GPUOpen::VulkanMemoryAllocator)




