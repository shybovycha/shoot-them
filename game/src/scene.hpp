#pragma once

class Scene
{
    public:
        virtual void load() = 0;

        virtual void render(float dt) = 0;

        virtual void unload() = 0;
};
