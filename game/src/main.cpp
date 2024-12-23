#include "application.hpp"

int main()
{
    auto app = std::make_unique<Application>();

    while (app->isRunning)
    {
        app->update();
    }

    return 0;
}
