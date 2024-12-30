cmake_minimum_required(VERSION 3.0)

project(heroic-epic-integration)

add_link_options(${LINK_OPTIONS})
add_executable(heroic-epic-integration ${CMAKE_SOURCE_DIR}/main.c)

target_link_libraries(heroic-epic-integration ${WIN_LIBRARIES})
