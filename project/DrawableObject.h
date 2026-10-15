//
// Created by xpolas on 10/15/26.
//

#ifndef PROJECT_DRAWABLEOBJECT_H
#define PROJECT_DRAWABLEOBJECT_H
#include <memory>
#include <vector>

#include "Model.h"
#include "ShaderProgram.h"
#include "Transformation.h"


class DrawableObject {
private:
    std::unique_ptr<Transformation> transformation;
    Model* model;
    ShaderProgram* shaderProgram;

public:
    DrawableObject(Model* model, ShaderProgram* shaderProgram, std::unique_ptr<Transformation> transformation);
    void draw() const;

};


#endif //PROJECT_DRAWABLEOBJECT_H
