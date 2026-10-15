//
// Created by xpolas on 10/15/26.
//

#include "Scene.h"

#include "DrawableObject.h"

void Scene::addObject(std::unique_ptr<DrawableObject> object) {
    objects.push_back(std::move(object));
}

void Scene::draw() const {
    for (auto& object : objects) {
        object->draw();
    }
}
