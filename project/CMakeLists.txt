cmake_minimum_required(VERSION 4.3)
project(project)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(project
        main.cpp
        Application.cpp

target_include_directories(cv02 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cv02 PRIVATE
        glfw
        OpenGL::GL
)
)
