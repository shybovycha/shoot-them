#pragma once

class Scene
{
public:
    virtual ~Scene() {
        cleanup();
    }

    virtual void init() = 0;

    virtual void render(float dt) = 0;

    virtual void cleanup() = 0;
};
