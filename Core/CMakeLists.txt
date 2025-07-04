add_subdirectory(UUID)
add_subdirectory(MAsset)
add_subdirectory(MThread)
add_subdirectory(MManager)
add_subdirectory(Utils)

add_library(Core INTERFACE)
target_link_libraries(Core INTERFACE MAsset UUID MThread MManager Utils)