cmake_minimum_required(VERSION 3.0)
find_package(PkgConfig)

project(heroic-epic-integration)

add_executable(heroic-epic-integration ${CMAKE_SOURCE_DIR}/main.cpp)

target_compile_features(heroic-epic-integration PUBLIC cxx_std_17)

target_link_libraries(heroic-epic-integration ${WIN_LIBRARIES})
