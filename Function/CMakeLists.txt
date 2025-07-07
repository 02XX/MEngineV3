add_subdirectory(MComponent)
add_subdirectory(MSystem)
add_subdirectory(Utils)

add_library(Function INTERFACE)
target_link_libraries(Function INTERFACE MComponent MSystem FunctionUtils)