cmake_minimum_required(VERSION 3.15...3.31)

project(ShadyMan VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CPP_COMPILER g++)

set(BIN_NAME main)

set(SOURCES main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${BIN_NAME} ${SOURCES})

set(LIBRARIES raylib GL m pthread dl rt)
target_link_libraries(${BIN_NAME} PUBLIC ${LIBRARIES})
