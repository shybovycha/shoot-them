#include "scenemanager.hpp"

void SceneManager::addScene(std::string name, std::unique_ptr<Scene> scene)
{
    scenes[name] = std::move(scene);
}

void SceneManager::setScene(std::string name)
{
    if (scenes.find(name) != scenes.end())
    {
        currentScene = scenes[name].get();
    }
}
