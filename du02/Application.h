//
// Created by pol0511 on 10/5/26.
//

#ifndef PROJECT_APPLICATION_H
#define PROJECT_APPLICATION_H
#include <memory>
#include <vector>


#include "Transformation.h"

class Rotation;
class DrawableObject;
//forward declarations
struct GLFWwindow; // -> main includes app.h, but does not know GLFWwindow
class Model;
class ShaderProgram;
class Scene;

class Application {
private:
    GLFWwindow *window = nullptr;
    std::unique_ptr<Model> model;
    std::unique_ptr<Model> treeModel;
    std::unique_ptr<Model> sphereModel;
    std::unique_ptr<ShaderProgram> shaderProgram;
    std::vector<std::unique_ptr<Scene>> scenes;
    std::size_t activeSceneIdx = 0;
    //Rotation* animatedRotation = nullptr;

    void createResources();
public:
    Application();
    ~Application();
    bool initialize();
    void run();

    void addScene(std::unique_ptr<Scene> scene);
    void setActiveScene(std::size_t idx);

};


#endif //PROJECT_APPLICATION_H
