//
// Created by xpolas on 10/5/26.
//

#ifndef PROJECT_APPLICATION_H
#define PROJECT_APPLICATION_H
#include <memory>

//forward declarations
struct GLFWwindow; // -> main includes app.h, but does not know GLFWwindow
class Model;
class ShaderProgram;

class Application {
private:
    GLFWwindow *window = nullptr;
    std::unique_ptr<Model> model;
    std::unique_ptr<ShaderProgram> shaderProgram;

    void createResources();
public:
    Application();
    ~Application();
    bool initialize();
    void run();

};


#endif //PROJECT_APPLICATION_H
