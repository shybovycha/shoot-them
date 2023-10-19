#include "scripting/LuaInterfaces.h"

int main()
{
    // create a Lua state
    lua_State* luaState = luaL_newstate();

    // load standard libs
    luaL_openlibs(luaState);

    initLuaAPIs(luaState);

    // load some code from Lua file
    int scriptLoadStatus = luaL_dofile(luaState, "resources/scenes/level1.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call function defined in Lua script
    luabridge::LuaRef onLoad = luabridge::getGlobal(luaState, "on_load");
    luabridge::LuaRef onRender = luabridge::getGlobal(luaState, "on_render");

    if (onLoad.isValid())
    {
        auto rc1 = onLoad();

        if (!rc1.wasOk())
            std::cerr << rc1.errorMessage();
    }

    if (onRender.isValid())
    {
        auto rc2 = onRender(0.1);

        if (!rc2.wasOk())
            std::cerr << rc2.errorMessage();
    }

    return 0;
}
