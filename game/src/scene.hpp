#pragma once

class Scene
{
public:
    virtual ~Scene() = default;

    virtual void handleWindowResizeEvent(uint32_t width, uint32_t height)
    {
    }

    virtual void handleKeyEvent(int key, int scancode, int action, int mods)
    {
    }

    virtual void handleMouseButtonEvent(int key, int action, int mods)
    {
    }

    virtual void handleCursorPositionEvent(double x, double y)
    {
    }

    virtual void render(float dt) = 0;
};
