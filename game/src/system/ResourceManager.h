#pragma once

#include <fmt/core.h>
#include <iostream>

class ResourceManager
{
public:
    void add3DModel(const std::string& name, const std::string& path)
    {
        std::cout << fmt::format("loaded 3d model '{}' from '{}'\n", name, path);
        // TODO
    }

    void addTexture(const std::string& name, const std::string& path)
    {
        std::cout << fmt::format("loaded texture '{}' from '{}'\n", name, path);
        // TODO
    }

    void addScene(const std::string& name, const std::string& path)
    {
        std::cout << fmt::format("loaded scene '{}' from '{}'\n", name, path);
        // TODO
    }
};
