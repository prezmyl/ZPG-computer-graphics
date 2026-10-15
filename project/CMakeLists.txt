cmake_minimum_required(VERSION 3.20)

project(project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(project
        main.cpp
        Application.cpp
        Model.cpp
        Shader.cpp
        ShaderProgram.cpp
        Transformation.h
        Translation.cpp
        Translation.h
        Rotation.cpp
        Rotation.h
)

target_include_directories(project PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(project PRIVATE
        glfw
        OpenGL::GL
)
# add directory to be compied upon compilation to the build directory
file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)