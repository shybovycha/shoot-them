#pragma once

extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
    #include <lualib.h>
}

#include <fmt/core.h>

#include <iostream>
#include <fstream>

#include <luabridge3/LuaBridge/LuaBridge.h>

#include "../math/Matrix3.h"
#include "../math/Matrix4.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

#include "../rendering/Camera.h"
#include "../rendering/Renderer.h"
#include "../system/InputManager.h"
#include "../system/ResourceManager.h"

class ScriptManager
{
public:
    ScriptManager()
    {
        // create a Lua state
        luaState = luaL_newstate();

        // load standard libs
        luaL_openlibs(luaState);
    }

    void initLuaAPIs(std::shared_ptr<Renderer> renderer, std::shared_ptr<ResourceManager> resources, std::shared_ptr<InputManager> inputs)
    {
        // define global classes: Vec2, Vec3, Vec4, Mat3, Mat4, Camera
        luabridge::getGlobalNamespace(luaState)
                .beginClass<Vector2>("Vec2")
                .addConstructor<void (*)(double, double)>()
                .addProperty("x", &Vector2::getX, &Vector2::setX)
                .addProperty("y", &Vector2::getY, &Vector2::setY)
                .addProperty("length", &Vector2::length)
                .addFunction("normalize", &Vector2::normalize)
                .addFunction("__add", &Vector2::operator+)
                .addFunction("__sub", &Vector2::operator-)
                .addFunction("__mul", &Vector2::operator*)
                .addFunction("__unm", &Vector2::negate)
                .addFunction("__tostring", &Vector2::toString)
                .endClass();

        luabridge::getGlobalNamespace(luaState)
                .beginClass<Vector3>("Vec3")
                .addConstructor<void (*)(double, double, double)>()
                .addProperty("x", &Vector3::getX, &Vector3::setX)
                .addProperty("y", &Vector3::getY, &Vector3::setY)
                .addProperty("z", &Vector3::getZ, &Vector3::setZ)
                .addProperty("length", &Vector3::length)
                .addFunction("normalize", &Vector3::normalize)
                .addFunction("__add", &Vector3::operator+)
                .addFunction("__sub", &Vector3::operator-)
                .addFunction("__mul", &Vector3::operator*)
                .addFunction("__unm", &Vector3::negate)
                .addFunction("__tostring", &Vector3::toString)
                .endClass();

        luabridge::getGlobalNamespace(luaState)
                .beginClass<Vector4>("Vec4")
                .addConstructor<void (*)(double, double, double, double)>()
                .addProperty("x", &Vector4::getX, &Vector4::setX)
                .addProperty("y", &Vector4::getY, &Vector4::setY)
                .addProperty("z", &Vector4::getZ, &Vector4::setZ)
                .addProperty("w", &Vector4::getW, &Vector4::setW)
                .addProperty("length", &Vector4::length)
                .addFunction("normalize", &Vector4::normalize)
                .addFunction("__add", &Vector4::operator+)
                .addFunction("__sub", &Vector4::operator-)
                .addFunction("__mul", &Vector4::operator*)
                .addFunction("__unm", &Vector4::negate)
                .addFunction("__tostring", &Vector4::toString)
                .endClass();

        luabridge::getGlobalNamespace(luaState)
                .beginClass<Matrix3>("Mat3")
                .endClass();

        luabridge::getGlobalNamespace(luaState)
                .beginClass<Matrix4>("Mat4")
                .endClass();

        luabridge::getGlobalNamespace(luaState)
                .beginClass<Camera>("Camera")
                .addConstructor<void (*)(Vector3, Vector3, Vector3, Vector3)>()
                .addProperty("view_matrix", &Camera::getViewMatrix)
                .addProperty("projection_matrix", &Camera::getProjectionMatrix)
                .addFunction("rotate", &Camera::rotate)
                .endClass();

        // define renderer singleton
        luabridge::getGlobalNamespace(luaState)
                .beginClass<Renderer>("Renderer")
                .addFunction("use_shader_program", &Renderer::useShaderProgram)
                .addFunction("use_framebuffer", &Renderer::useFramebuffer)
                .addFunction("set_uniform",
                             luabridge::overload<const std::string&, Vector2>(&Renderer::setUniform),
                             luabridge::overload<const std::string&, Vector3>(&Renderer::setUniform),
                             luabridge::overload<const std::string&, Matrix3>(&Renderer::setUniform),
                             luabridge::overload<const std::string&, Matrix4>(&Renderer::setUniform))
                .addFunction("set_texture", &Renderer::setTexture)
                .addFunction("draw_3d_model", &Renderer::draw3DModel)
                .endClass();

        auto rg1 = luabridge::push(luaState, renderer.get());
        lua_setglobal(luaState, "renderer");

        // define script manager singleton
        luabridge::getGlobalNamespace(luaState)
                .beginClass<ScriptManager>("ScriptManager")
                .addFunction("load_scene", &ScriptManager::loadScene)
                .endClass();

        auto rg4 = luabridge::push(luaState, this);
        lua_setglobal(luaState, "scenes");

        // define resource manager singleton
        luabridge::getGlobalNamespace(luaState)
                .beginClass<ResourceManager>("ResourceManager")
                .addFunction("add_3d_model", &ResourceManager::add3DModel)
                .addFunction("add_texture", &ResourceManager::addTexture)
                .endClass();

        auto rg2 = luabridge::push(luaState, resources.get());
        lua_setglobal(luaState, "resources");

        // define input manager singleton
        luabridge::getGlobalNamespace(luaState)
                .beginClass<InputManager>("InputManager")
                .addProperty("mouse_position", &InputManager::getMousePosition)
                .addFunction("is_mouse_button_pressed", &InputManager::isMouseButtonPressed)
                .endClass();

        auto rg3 = luabridge::push(luaState, inputs.get());
        lua_setglobal(luaState, "inputs");

        // define global constants: MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT
        auto MOUSE_BUTTON_LEFT = 1;
        auto MOUSE_BUTTON_RIGHT = 2;

        auto r1 = luabridge::push(luaState, MOUSE_BUTTON_LEFT);
        lua_setglobal(luaState, "MOUSE_BUTTON_LEFT");

        auto r2 = luabridge::push(luaState, MOUSE_BUTTON_RIGHT);
        lua_setglobal(luaState, "MOUSE_BUTTON_RIGHT");

        // TODO: move this to SceneManager
        // scene scope data table
        auto sceneData = luabridge::newTable(luaState);
        auto r3 = luabridge::push(luaState, sceneData);
        lua_setglobal(luaState, "scene_data");
    }

    void loadScript(const std::string& path)
    {
        // load some code from Lua file
        int scriptLoadStatus = luaL_dofile(luaState, path.c_str());

        // define error reporter for any Lua error
        reportLuaErrors(scriptLoadStatus);
    }

    luabridge::LuaRef loadScene(const std::string& path)
    {
        std::ifstream f{ path };
        std::string script{ std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>() };
        std::string code = script + "\nreturn { on_load = on_load, on_render = on_render }";

        std::cout << fmt::format("[C++ INFO] loading scene {}\n", path);

        // run the new scene code
        int scriptLoadStatus = luaL_dostring(luaState, code.c_str());

        // define error reporter for any Lua error
        reportLuaErrors(scriptLoadStatus);

        auto sceneFn = luabridge::get<luabridge::LuaRef>(luaState, 0).value();

        return sceneFn()[0];
    }

    luabridge::LuaRef getGlobal(const std::string& name)
    {
        return luabridge::getGlobal(luaState, name.c_str());
    }

    void setGlobal(const std::string& name, luabridge::LuaRef value)
    {
        auto r = luabridge::push(luaState, value);
        lua_setglobal(luaState, name.c_str());
    }

private:
    void reportLuaErrors(int status)
    {
        if (status == 0)
        {
            return;
        }

        std::cerr << fmt::format("[LUA ERROR] {}\n", lua_tostring(luaState, -1));

        // remove error message from Lua state
        lua_pop(luaState, 1);
    }

private:
    lua_State* luaState;
};
