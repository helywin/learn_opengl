cmake_minimum_required(VERSION 3.18)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 17)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(src)
