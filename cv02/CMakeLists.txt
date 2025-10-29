cmake_minimum_required(VERSION 3.16)
project(cv02)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
# GLM je header-only; pokud nenajde config, nevadí (na Fedoře bývá glm-devel OK)

include_directories(
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        # ${GLM_INCLUDE_DIRS}  # volitelné, často není potřeba
)

add_executable(cv02 main.cpp)

target_link_libraries(cv02
        ${GLFW_LIBRARIES}
        GLEW::GLEW
        OpenGL::GL
)
