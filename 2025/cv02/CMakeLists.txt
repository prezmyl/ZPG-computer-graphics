cmake_minimum_required(VERSION 3.20)

project(cv02)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(cv02
        main.cpp
)

target_include_directories(cv02 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cv02 PRIVATE
        glfw
        OpenGL::GL
)

# Zkopíruje shaders do build adresáře,
# aby fungovala cesta "shaders/basic.vert" i při spuštění z CLionu.
file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)