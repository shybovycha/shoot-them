#pragma once

#include <memory>

#include "QueueAction.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "StateManager.h"

#include "../game/GameState.h"

template<class T>
class Application
{
public:
    Application(std::shared_ptr<Renderer> _renderer, std::shared_ptr<ResourceManager> _resourceManager);

    void run();

private:
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<ResourceManager> resourceManager;
};

template class Application<GameState>;
