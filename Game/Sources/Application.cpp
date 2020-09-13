#include "Application.h"

Application::Application(std::shared_ptr<Renderer> _renderer, std::shared_ptr<GameState> _gameState, std::shared_ptr<ResourceManager> _resourceManager) :
    renderer(std::move(_renderer)),
    gameState(std::move(_gameState)),
    inputHandler(std::make_shared<InputHandler>(gameState)),
    resourceManager(std::move(_resourceManager))
{}

void Application::run() {
    gameState->setLevels(resourceManager->loadLevels());

    renderer->init(resourceManager->loadSettings());

    while (renderer->isRunning()) {
        renderer->processActionQueue();
        renderer->render();
    }

    renderer->shutdown();
}
