//
// Created by xpolas on 10/15/26.
//

#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H
#include <memory>
#include <vector>

#include "Camera.h"


class Animation;
class DrawableObject;
//class Camera;

class Scene {
private:
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::vector<std::unique_ptr<Animation>> animations;
    Camera camera;

public:
    Scene(const Camera camera);
    ~Scene();
    void addObject(std::unique_ptr<DrawableObject> object);
    void addAnimation(std::unique_ptr<Animation> animation);
    void draw() const;
    void update(float time);
    Camera& getCamera();
};



#endif //PROJECT_SCENE_H
