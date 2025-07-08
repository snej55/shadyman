cmake_minimum_required(VERSION 3.15...3.31)

project(ShadyMan VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CPP_COMPILER g++)

set(BIN_NAME main)

set(SOURCES main.cpp src/game.hpp src/constants.hpp src/game.cpp src/tiles.hpp src/tiles.cpp src/vec2.hpp 
src/assets.hpp src/assets.cpp src/player.hpp src/player.cpp src/util.hpp src/anim.hpp src/entities.hpp src/entities.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${BIN_NAME} ${SOURCES})

set(LIBRARIES raylib GL m pthread dl rt)
target_link_libraries(${BIN_NAME} PUBLIC ${LIBRARIES})

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
add_dependencies(${BIN_NAME} copy_assets)