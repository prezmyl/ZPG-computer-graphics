cmake_minimum_required(VERSION 3.20)
project(cv04_du LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Najdi knihovny
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# GLFW: zkus více názvů targetu, jinak spadni na -lglfw
find_package(glfw3 QUIET)
set(GLFW_TARGET "")
if (TARGET glfw)
    set(GLFW_TARGET glfw)
elseif (TARGET glfw3)
    set(GLFW_TARGET glfw3)
elseif (TARGET GLFW::GLFW)
    set(GLFW_TARGET GLFW::GLFW)
else()
    set(GLFW_TARGET glfw)  # spoléháme na -lglfw z linkeru
endif()

# Zdroje – rekurzivně
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        src/*.cpp
)

# >>> Target MUSÍ být nadefinovaný před target_* příkazy <<<
add_executable(cv04_du ${SOURCES})

# Po sestavení zkopíruj assets vedle binárky (do build diru)
add_custom_command(TARGET cv04_du POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:cv04_du>/assets)


target_include_directories(cv04_du PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cv04_du PRIVATE
        OpenGL::GL
        GLEW::GLEW
        ${GLFW_TARGET}
)

# Linuxové dodatky
if (UNIX AND NOT APPLE)
    target_link_libraries(cv04_du PRIVATE dl pthread)
endif()
