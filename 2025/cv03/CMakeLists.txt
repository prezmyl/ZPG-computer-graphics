cmake_minimum_required(VERSION 3.16)
project(cv03)                       # <<< sjednoť s názvem složky / konfigurace

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)         # poskytne imported target GLEW::GLEW
# GLM je header-only; obvykle stačí mít nainstalované hlavičky (glm-devel)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        # ${GLM_INCLUDE_DIRS}      # obvykle netřeba, GLM bývá v /usr/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${GLFW_LIBRARIES}
        GLEW::GLEW
        OpenGL::GL
)

# (volitelné, pokud pkg-config dodá extra flagy)
target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS_OTHER})
target_link_options(${PROJECT_NAME} PRIVATE ${GLFW_LDFLAGS_OTHER})
