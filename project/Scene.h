//
// Created by xpolas on 10/15/26.
//

#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H
#include <memory>
#include <vector>


class DrawableObject;

class Scene {
private:
    std::vector<std::unique_ptr<DrawableObject>> objects;

public:
    ~Scene();
    void addObject(std::unique_ptr<DrawableObject> object);
    void draw() const;
};



#endif //PROJECT_SCENE_H
