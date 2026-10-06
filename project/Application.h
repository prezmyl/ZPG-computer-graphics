//
// Created by xpolas on 10/5/26.
//

#ifndef PROJECT_APPLICATION_H
#define PROJECT_APPLICATION_H

struct GLFWwindow; //forward declaration -> main includes app.h, but does not know GLFWwindow

class Application {
public:
    bool initialize();
    void run();

private:
    GLFWwindow *window = nullptr;
};


#endif //PROJECT_APPLICATION_H
