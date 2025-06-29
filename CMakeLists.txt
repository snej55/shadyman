cmake_minimum_required(VERSION 3.15...3.31)

project(lawn-mower-simulator VERSION 1.0
                             LANGUAGES CXX)

set(CMAKE_CPP_COMPILER g++)

set(BIN_NAME main)

set(SOURCES main.cpp src/constants.hpp src/core/util.hpp
        src/app.h
        src/core/texture.h
        src/core/vec2.hpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
#link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(${BIN_NAME} ${SOURCES})

set(SDL2_LIBRARIES SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)
target_link_libraries(${BIN_NAME} PUBLIC ${SDL2_LIBRARIES})
