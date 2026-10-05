cmake_minimum_required(VERSION 3.16)
project(cv04)                       # ← nový název targetu/binárky

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)

add_executable(${PROJECT_NAME}
        main.cpp
        Shader.cpp Shader.h
        ShaderProgram.cpp ShaderProgram.h
        DrawableObject.cpp DrawableObject.h
        PNArrayModel.h Model.h
        Transform.cpp Transform.h TransformChain.h
        Scene.h Scene.cpp
        Scenes.h            # pokud Scene.cpp nemáš, vyhoď ten soubor z listu
        # + případné další .cpp/.h které máš v projektu
)

target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES} GLEW::GLEW OpenGL::GL)
target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS_OTHER})
target_link_options(${PROJECT_NAME} PRIVATE ${GLFW_LDFLAGS_OTHER})
