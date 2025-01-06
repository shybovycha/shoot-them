#include "scenemanager.hpp"

void SceneManager::addScene(SceneID id, std::unique_ptr<Scene> scene)
{
    scenes[id] = std::move(scene);
}

void SceneManager::setScene(SceneID id)
{
    if (scenes.find(id) != scenes.end())
    {
        currentScene = scenes[id].get();
    }
}
