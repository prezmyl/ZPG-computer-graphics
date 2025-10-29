#!/usr/bin/env bash
set -euo pipefail

root="$(pwd)"
if [[ ! -f CMakeLists.txt ]]; then
  echo "❌ Spusť mě v adresáři cv04_du (nenašel jsem CMakeLists.txt)." >&2
  exit 1
fi

echo "==> 0) Záloha CMakeLists.txt"
cp -f CMakeLists.txt CMakeLists.txt.bak || true

echo "==> 1) Hromadná oprava include cest"
# Mapování starých relativních include na nové se správným prefixem
declare -A MAP=(
  ['#include "DrawableObject.h"']='#include "core/DrawableObject.h"'
  ['#include "Model.h"']='#include "core/Model.h"'
  ['#include "TriangleModel.h"']='#include "models/TriangleModel.h"'
  ['#include "TransformChain.h"']='#include "math/TransformChain.h"'
  ['#include "Translate.h"']='#include "math/Translate.h"'
  ['#include "Rotate.h"']='#include "math/Rotate.h"'
  ['#include "Scale.h"']='#include "math/Scale.h"'
  ['#include "Scene.h"']='#include "scene/Scene.h"'
  ['#include "Scenes.h"']='#include "scene/Scenes.h"'
  ['#include "ShaderProgram.h"']='#include "gfx/ShaderProgram.h"'
  # nové moduly v gfx (pro jistotu sjednotíme prefix)
  ['#include "ShaderStage.h"']='#include "gfx/ShaderStage.h"'
  ['#include "Technique.h"']='#include "gfx/Technique.h"'
  ['#include "Material.h"']='#include "gfx/Material.h"'
  ['#include "Ubo.h"']='#include "gfx/Ubo.h"'
  ['#include "Camera.h"']='#include "gfx/Camera.h"'
  ['#include "Light.h"']='#include "gfx/Light.h"'
  ['#include "CameraUboBinder.h"']='#include "gfx/CameraUboBinder.h"'
  ['#include "LightUboBinder.h"']='#include "gfx/LightUboBinder.h"'
)

# projdeme všechny .cpp/.h v src
mapfile -t FILES < <(find src -type f \( -name '*.cpp' -o -name '*.h' \))
for f in "${FILES[@]}"; do
  for from in "${!MAP[@]}"; do
    to="${MAP[$from]}"
    if grep -Fq "$from" "$f"; then
      sed -i "s|$from|$to|g" "$f"
      echo "  • $f: $from → $to"
    fi
  done
done

echo "==> 2) Opravy překlepů a GLM includes"
# #gnclude → #include
grep -RIl '^#gnclude' src | while read -r f; do
  sed -i 's/^#gnclude/#include/g' "$f"
  echo "  • $f: opraven #gnclude → #include"
done

# sjednocení GLM matrix_transform (ponecháme gtc)
grep -RIl '<glm/.*/matrix_transform\\.hpp>' src | while read -r f; do
  sed -i 's|<glm/.*/matrix_transform.hpp>|<glm/gtc/matrix_transform.hpp>|g' "$f"
  echo "  • $f: sjednocen include GLM matrix_transform.hpp"
done

echo "==> 3) Úklid duplicitních lomítek v includech (// → /)"
# někdy se sejde "gfx//ShaderProgram.h" – opravíme
grep -RIl '#include "' src | while read -r f; do
  sed -i 's|#include \"\\(.*\\)//|#include \"\\1/|g' "$f"
done

echo "==> 4) Oprava include v main.cpp (app)"
if [[ -f src/app/main.cpp ]]; then
  sed -i 's/#include \"Scene.h\"/#include \"scene\\/Scene.h\"/g' src/app/main.cpp
  sed -i 's/#include \"Scenes.h\"/#include \"scene\\/Scenes.h\"/g' src/app/main.cpp
  sed -i 's/#include \"ShaderProgram.h\"/#include \"gfx\\/ShaderProgram.h\"/g' src/app/main.cpp
  # pro jistotu i Model/TriangleModel
  sed -i 's/#include \"TriangleModel.h\"/#include \"models\\/TriangleModel.h\"/g' src/app/main.cpp
  sed -i 's/#include \"Model.h\"/#include \"core\\/Model.h\"/g' src/app/main.cpp
fi

echo "==> 5) Přepsání CMakeLists.txt na robustní variantu"
cat > CMakeLists.txt << 'CMAKE'
cmake_minimum_required(VERSION 3.20)
project(cv04_du LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===== Závislosti =====
find_package(OpenGL REQUIRED)

# GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
  find_package(GLFW3 REQUIRED) # některé distribuce exportují takto
endif()

# GLEW
find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
  find_package(GLEW REQUIRED)
endif()

# GLM (header-only target glm::glm)
find_package(glm QUIET)
if(NOT glm_FOUND)
  find_package(GLM REQUIRED)
endif()

# ===== Soubory =====
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ===== Linkování =====
set(_GLFW_TARGET "")
if(TARGET glfw)
  set(_GLFW_TARGET glfw)
elseif(TARGET glfw3)
  set(_GLFW_TARGET glfw3)
elseif(TARGET GLFW::GLFW)
  set(_GLFW_TARGET GLFW::GLFW)
endif()

set(_GLEW_TARGET "")
if(TARGET GLEW::GLEW)
  set(_GLEW_TARGET GLEW::GLEW)
elseif(TARGET glew_s)
  set(_GLEW_TARGET glew_s)
elseif(TARGET glew)
  set(_GLEW_TARGET glew)
endif()

set(_GLM_TARGET "")
if(TARGET glm::glm)
  set(_GLM_TARGET glm::glm)
elseif(TARGET glm)
  set(_GLM_TARGET glm)
endif()

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    OpenGL::GL
    ${_GLEW_TARGET}
    ${_GLFW_TARGET}
    ${_GLM_TARGET}
)

# Některé Fedora/Arch balíčky potřebují navíc dl/pthread
if (UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE dl pthread)
endif()
CMAKE

echo "==> 6) Shrnutí změn"
echo "  • include cesty sjednoceny (core/, math/, scene/, gfx/, models/)"
echo "  • opraveny překlepy (#gnclude)"
echo "  • aktualizován CMakeLists.txt (OpenGL+GLEW+GLFW+GLM, C++20, glob-recurse)"

echo "==> 7) Tip: přegenerování build systému"
echo "   mkdir -p cmake-build-debug && cd cmake-build-debug"
echo "   cmake -DCMAKE_BUILD_TYPE=Debug .."
echo "   cmake --build . -j"

echo "HOTOVO ✅"
