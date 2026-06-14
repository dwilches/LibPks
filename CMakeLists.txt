cmake_minimum_required(VERSION 4.2)
project(LibPks)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/libpks)
add_subdirectory(src/apps)
add_subdirectory(tests)
