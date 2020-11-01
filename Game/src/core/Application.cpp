#include "Application.h"

template<class T>
Application<T>::Application(std::shared_ptr<Renderer> _renderer, std::shared_ptr<ResourceManager> _resourceManager)
        : renderer(_renderer), resourceManager(_resourceManager)
{
}

template<class T>
void Application<T>::run()
{
    StateManager<T>::getInstance()->dispatch(new LevelsLoadedAction(resourceManager->loadLevels()));

    renderer->init(resourceManager->loadSettings());

    while (renderer->isRunning())
    {
        StateManager<T>::getInstance()->processActionQueue();
        renderer->render();
    }

    renderer->shutdown();
}
