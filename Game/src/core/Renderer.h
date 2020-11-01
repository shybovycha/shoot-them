#pragma once

#include <memory>

#include "Settings.h"

class Renderer
{
public:
    virtual void init(std::shared_ptr<Settings> settings) = 0;

    virtual void render() = 0;

    virtual void shutdown() = 0;

    virtual bool isRunning() = 0;
};
