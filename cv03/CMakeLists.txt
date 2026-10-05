cmake_minimum_required(VERSION 3.20)

project(cv03)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(cv03
        main.cpp
)

target_include_directories(cv03 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cv03 PRIVATE
        glfw
        OpenGL::GL
)