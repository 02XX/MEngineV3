file(GLOB_RECURSE RESOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${SOURCE_FILES} PROPERTIES CMAKE_CONFIGURE_DEPENDS ON)


find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)

add_library(Asset ${RESOURCE})
target_include_directories(Asset PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(Asset PUBLIC glm::glm)
target_link_libraries(Asset PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(Asset PUBLIC Common)
target_link_libraries(Asset PUBLIC assimp::assimp)
target_link_libraries(Asset PUBLIC UUID Thread Platform Manager Math Enums)




