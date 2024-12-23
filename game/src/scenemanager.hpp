#pragma once

#include "stdafx.hpp"
#include "scene.hpp"

struct SceneManager {
    std::map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* currentScene;

    void addScene(std::string name, std::unique_ptr<Scene> scene);

    void setScene(std::string name);
};
