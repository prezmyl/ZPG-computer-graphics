//
// Created by xpolas on 10/15/26.
//

#include "Scene.h"

#include "DrawableObject.h"
#include <utility>

#include "Animation.h"

Scene::~Scene() = default;

void Scene::addObject(std::unique_ptr<DrawableObject> object) {
    objects.push_back(std::move(object));
}

void Scene::addAnimation(std::unique_ptr<Animation> animation) {
    animations.push_back(std::move(animation));
}

void Scene::draw() const {
    for (auto& object : objects) {
        object->draw();
    }
}

void Scene::update(float time) {
    for (auto& animation : animations) {
        animation->update(time);
    }
}
