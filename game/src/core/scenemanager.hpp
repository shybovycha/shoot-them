#pragma once

#include "../stdafx.hpp"

#include "scene.hpp"
#include "scenes.hpp"

class SceneManager
{
public:
    void addScene(SceneID id, std::unique_ptr<Scene> scene);

    void setScene(SceneID id);

public:
    std::map<SceneID, std::unique_ptr<Scene>> scenes;
    Scene* currentScene;
};
