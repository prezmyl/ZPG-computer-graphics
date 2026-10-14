//
// Created by xpolas on 10/14/26.
//

#ifndef PROJECT_TRANSLATION_H
#define PROJECT_TRANSLATION_H
#include "Transformation.h"


class Translation : public Transformation {
private:
    glm::vec3 translation;

public:
    Translation(glm::vec3 translation);
    //virtual ~Translation(); nemusi byt nepotrebuje li specialni clean up, uz je v predkovi
    virtual glm::mat4 getMatrix() const override;

};


#endif //PROJECT_TRANSLATION_H
