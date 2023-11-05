#include "scripting/ScriptManager.h"

int main()
{
    auto scriptManager = std::make_shared<ScriptManager>();

    auto renderer = std::make_shared<Renderer>();
    auto resources = std::make_shared<ResourceManager>();
    auto inputs = std::make_shared<InputManager>();

    scriptManager->initLuaAPIs(renderer, resources, inputs);

    // load some code from Lua file
    scriptManager->loadScript("resources/scenes/main.lua");

    // call function defined in Lua script
    auto onLoad = scriptManager->getGlobal("on_load");
    auto onRender = scriptManager->getGlobal("on_render");

    if (onLoad.isValid())
    {
        auto rc1 = onLoad();

        if (!rc1.wasOk())
            std::cerr << rc1.errorMessage();
    } else {
        std::cerr << "Could not load `on_load` function in the scene";
    }

    if (onRender.isValid())
    {
        auto rc2 = onRender(0.1);

        if (!rc2.wasOk())
            std::cerr << rc2.errorMessage();
    } else {
        std::cerr << "Could not load `on_render` function in the scene";
    }

    return 0;
}
