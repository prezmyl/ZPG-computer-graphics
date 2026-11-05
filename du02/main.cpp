//
// Created by pol0511 on 10/5/26.
//
#include <cstdlib>
#include "Application.h"

int main()
{
    Application app;

    if (!app.initialize())
        return EXIT_FAILURE;

    app.run();

    return EXIT_SUCCESS;
}