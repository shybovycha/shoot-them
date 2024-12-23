#include "application.hpp"

int main()
{
    auto app = std::make_unique<Application>();

    app->init();

    while (app->isRunning) {
        app->update();
    }

    app->cleanup();

    return 0;
}
