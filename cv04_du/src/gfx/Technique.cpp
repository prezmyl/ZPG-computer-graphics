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
