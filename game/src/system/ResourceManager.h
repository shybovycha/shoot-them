#pragma once

#include <format>
#include <iostream>

class ResourceManager
{
public:
    void add3DModel(const std::string& name, const std::string& path)
    {
        std::cout << std::format("loaded 3d model '{}' from '{}'\n", name, path);
        // TODO
    }

    void addTexture(const std::string& name, const std::string& path)
    {
        std::cout << std::format("loaded texture '{}' from '{}'\n", name, path);
        // TODO
    }

    void addScene(const std::string& name, const std::string& path)
    {
        std::cout << std::format("loaded scene '{}' from '{}'\n", name, path);
        // TODO
    }
};
