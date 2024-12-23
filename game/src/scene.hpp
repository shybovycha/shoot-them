#pragma once

class Scene
{
public:
    virtual ~Scene() = default;

    virtual void render(float dt) = 0;
};
