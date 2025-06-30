file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
# find_package(fmt CONFIG REQUIRED)



add_library(Common ${RESOURCE})

target_include_directories(Common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Common PUBLIC glm::glm-header-only)
target_link_libraries(Common PUBLIC spdlog::spdlog)
target_link_libraries(Common PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(Common PUBLIC magic_enum::magic_enum)
# target_link_libraries(Common PUBLIC fmt::fmt-header-only)
