//
// Created by xpolas on 10/5/26.
//

#ifndef PROJECT_APPLICATION_H
#define PROJECT_APPLICATION_H


class Application {
public:
    bool initialize();
    void run();

private:
    GLFWwindow *window = nullptr;
};


#endif //PROJECT_APPLICATION_H
