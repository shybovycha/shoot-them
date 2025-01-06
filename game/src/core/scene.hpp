#pragma once

class WindowManager;
class SceneManager;

class Scene
{
public:
    Scene(WindowManager* windowManager, SceneManager* sceneManager)
        : windowManager(windowManager),
          sceneManager(sceneManager)
    {
    }

    virtual ~Scene() = default;

    virtual void handleWindowResizeEvent(int width, int height)
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

protected:
    WindowManager* windowManager;
    SceneManager* sceneManager;
};
