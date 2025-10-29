# --- patch_fix.sh ---
set -euo pipefail

# 1) Přesuň AbstractTransformation.h do include-rootu (src/)
if [[ -f "AbstractTransformation.h" ]]; then
  echo "[1] Moving AbstractTransformation.h -> src/math/AbstractTransformation.h"
  mkdir -p src/math
  if [[ -d .git ]]; then git mv -f AbstractTransformation.h src/math/AbstractTransformation.h; else mv -f AbstractTransformation.h src/math/AbstractTransformation.h; fi
fi

# 2) Oprav include cesty na AbstractTransformation.h ve všech math/ hlavičkách a zdrojích
echo "[2] Rewriting includes of AbstractTransformation.h -> math/AbstractTransformation.h"
grep -RIl --include='*.h' --include='*.cpp' 'AbstractTransformation.h' src | while read -r f; do
  sed -i 's|#include "AbstractTransformation.h"|#include "math/AbstractTransformation.h"|g' "$f"
done

# 3) Pro jistotu oprav případné relikty "src/gfx/ShaderProgram.h" v mainu
if [[ -f src/app/main.cpp ]]; then
  sed -i 's|#include "src/gfx/ShaderProgram.h"|#include "gfx/ShaderProgram.h"|g' src/app/main.cpp
fi

# 4) Přepiš Technique.cpp na verzi kompatibilní s ShaderProgram::fromSources()
#    (jednoduše načte VS/FS ze souborů a vytvoří Technique bez UBO bindů)
if [[ -f src/gfx/Technique.cpp ]]; then
  echo "[4] Replacing src/gfx/Technique.cpp with compatible implementation"
  cat > src/gfx/Technique.cpp <<'EOF'
#include "Technique.h"
#include <fstream>
#include <sstream>
#include <memory>

static std::string slurp(const std::string& path){
    std::ifstream f(path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::shared_ptr<Technique> Technique::createFromFiles(const std::string& vs, const std::string& fs, std::string* /*err*/){
    const std::string vsSrc = slurp(vs);
    const std::string fsSrc = slurp(fs);
    ShaderProgram sp = ShaderProgram::fromSources(vsSrc.c_str(), fsSrc.c_str());
    auto t = std::make_shared<Technique>();
    t->program = std::make_shared<ShaderProgram>(std::move(sp));
    return t;
}
EOF
fi

echo "[OK] Patch hotový. Teď přegeneruj build:"
echo "     mkdir -p cmake-build-debug && cd cmake-build-debug"
echo "     cmake -DCMAKE_BUILD_TYPE=Debug .."
echo "     cmake --build . -j"
