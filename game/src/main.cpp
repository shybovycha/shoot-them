extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
    #include <lualib.h>
}

#include <iostream>
#include <format>

#include <luabridge3/LuaBridge/LuaBridge.h>

void report_errors(lua_State* luaState, int status)
{
    if (status == 0)
    {
        return;
    }

    std::cerr << std::format("[LUA ERROR] {}\n", lua_tostring(luaState, -1));

    // remove error message from Lua state
    lua_pop(luaState, 1);
}

class Vector2
{
public:
    Vector2()
        : m_x(0.0), m_y(0.0)
    {
    }

    Vector2(double x, double y)
        : m_x(x), m_y(y)
    {
    }

    Vector2 operator+(Vector2 other) const
    {
        return Vector2(m_x + other.m_x, m_y + other.m_y);
    }

    Vector2 operator-(Vector2 other) const
    {
        return Vector2(m_x - other.m_x, m_y - other.m_y);
    }

    Vector2 operator*(double f) const
    {
        return Vector2(m_x * f, m_y * f);
    }

    double length() const
    {
        return std::sqrt(m_x * m_x + m_y * m_y);
    }

    Vector2 normalize() const
    {
        const auto l = length();

        return Vector2(m_x / l, m_y / l);
    }

    Vector2 negate() const
    {
        return Vector2(-m_x, -m_y);
    }

    double getX() const
    {
        return m_x;
    }

    double getY() const
    {
        return m_y;
    }

    void setX(double x)
    {
        m_x = x;
    }

    void setY(double y)
    {
        m_y = y;
    }

private:
    double m_x;
    double m_y;
};

class Vector3
{
public:
    Vector3()
        : m_x(0.0), m_y(0.0), m_z(0.0)
    {
    }

    Vector3(double x, double y, double z)
        : m_x(x), m_y(y), m_z(z)
    {
    }

    Vector3 operator+(Vector3 other) const
    {
        return Vector3(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
    }

    Vector3 operator-(Vector3 other) const
    {
        return Vector3(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
    }

    Vector3 operator*(double f) const
    {
        return Vector3(m_x * f, m_y * f, m_z * f);
    }

    double length() const
    {
        return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    }

    Vector3 normalize() const
    {
        const auto l = length();

        return Vector3(m_x / l, m_y / l, m_z / l);
    }

    Vector3 negate() const
    {
        return Vector3(-m_x, -m_y, -m_z);
    }

    double getX() const
    {
        return m_x;
    }

    double getY() const
    {
        return m_y;
    }

    double getZ() const
    {
        return m_z;
    }

    void setX(double x)
    {
        m_x = x;
    }

    void setY(double y)
    {
        m_y = y;
    }

    void setZ(double z)
    {
        m_z = z;
    }

private:
    double m_x;
    double m_y;
    double m_z;
};

class Vector4
{
public:
    Vector4()
        : m_x(0.0), m_y(0.0), m_z(0.0), m_w(0.0)
    {
    }

    Vector4(double x, double y, double z, double w)
        : m_x(x), m_y(y), m_z(z), m_w(w)
    {
    }

    Vector4 operator+(Vector4 other) const
    {
        return Vector4(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z, m_w + other.m_w);
    }

    Vector4 operator-(Vector4 other) const
    {
        return Vector4(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z, m_w - other.m_w);
    }

    Vector4 operator*(double f) const
    {
        return Vector4(m_x * f, m_y * f, m_z * f, m_w * f);
    }

    double length() const
    {
        return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);
    }

    Vector4 normalize() const
    {
        const auto l = length();

        return Vector4(m_x / l, m_y / l, m_z / l, m_w / l);
    }

    Vector4 negate() const
    {
        return Vector4(-m_x, -m_y, -m_z, -m_w);
    }

    double getX() const
    {
        return m_x;
    }

    double getY() const
    {
        return m_y;
    }

    double getZ() const
    {
        return m_z;
    }

    double getW() const
    {
        return m_w;
    }

    void setX(double x)
    {
        m_x = x;
    }

    void setY(double y)
    {
        m_y = y;
    }

    void setZ(double z)
    {
        m_z = z;
    }

    void setW(double w)
    {
        m_w = w;
    }

private:
    double m_x;
    double m_y;
    double m_z;
    double m_w;
};

class Matrix3
{
    // TODO
};

class Matrix4
{
    // TODO
};

class Camera
{
public:
    Camera(Vector3 up, Vector3 forward, Vector3 right, Vector3 position)
        : m_up(up), m_forward(forward), m_right(right), m_position(position)
    {
    }

    Matrix4 getViewMatrix() const
    {
        return Matrix4();
    }

    Matrix4 getProjectionMatrix() const
    {
        return Matrix4();
    }

    void rotate(double dx, double dy)
    {

    }

private:
    Vector3 m_up;
    Vector3 m_forward;
    Vector3 m_right;
    Vector3 m_position;
};

class ResourceManager
{
public:
    void add3DModel(const std::string& name, const std::string& path)
    {
        // TODO
    }

    void addTexture(const std::string& name, const std::string& path)
    {
        // TODO
    }
};

class InputManager
{
public:
    Vector2 getMousePosition() const
    {
        return Vector2();
    }

    bool isMouseButtonPressed(int button) const
    {
        return false;
    }
};

class Renderer
{
public:
    void useShaderProgram(const std::string& resourceName)
    {
    }

    void useFramebuffer(const std::string& resourceName)
    {
    }

    void setUniform(const std::string& variableName, Vector2 value)
    {
    }

    void setUniform(const std::string& variableName, Vector3 value)
    {
    }

    void setUniform(const std::string& variableName, Matrix3 value)
    {
    }

    void setUniform(const std::string& variableName, Matrix4 value)
    {
    }

    void setTexture(const std::string& variableName, const std::string& resourceName)
    {
    }

    void draw3DModel(const std::string& resourceName)
    {
    }
};

int main()
{
    // create a Lua state
    lua_State* luaState = luaL_newstate();

    // load standard libs
    luaL_openlibs(luaState);

    // define global classes: Vec2, Vec3, Vec4, Mat3, Mat4, Camera
    luabridge::getGlobalNamespace(luaState)
            // .beginNamespace("scene")
            .beginClass<Vector2>("Vec2")
            .addConstructor<void (*)(double, double)>()
            .addProperty("x", &Vector2::getX, &Vector2::setX)
            .addProperty("y", &Vector2::getY, &Vector2::setY)
            .addProperty("length", &Vector2::length)
            .addFunction("normalize", &Vector2::normalize)
            .addFunction("__add", &Vector2::operator+)
            .addFunction("__mul", &Vector2::operator*)
            .addFunction("__unm", &Vector2::negate)
            .endClass();
        // .endNamespace();

    luabridge::getGlobalNamespace(luaState)
            //.beginNamespace("scene")
            .beginClass<Vector3>("Vec3")
            .addConstructor<void (*)(double, double, double)>()
            .addProperty("x", &Vector3::getX, &Vector3::setX)
            .addProperty("y", &Vector3::getY, &Vector3::setY)
            .addProperty("z", &Vector3::getZ, &Vector3::setZ)
            .addProperty("length", &Vector3::length)
            .addFunction("normalize", &Vector3::normalize)
            .addFunction("__add", &Vector3::operator+)
            .addFunction("__mul", &Vector3::operator*)
            .addFunction("__unm", &Vector3::negate)
            .endClass();
       // .endNamespace();

    luabridge::getGlobalNamespace(luaState)
            //.beginNamespace("scene")
            .beginClass<Vector4>("Vec4")
            .addConstructor<void (*)(double, double, double, double)>()
            .addProperty("x", &Vector4::getX, &Vector4::setX)
            .addProperty("y", &Vector4::getY, &Vector4::setY)
            .addProperty("z", &Vector4::getZ, &Vector4::setZ)
            .addProperty("w", &Vector4::getW, &Vector4::setW)
            .addProperty("length", &Vector4::length)
            .addFunction("normalize", &Vector4::normalize)
            .addFunction("__add", &Vector4::operator+)
            .addFunction("__mul", &Vector4::operator*)
            .addFunction("__unm", &Vector4::negate)
            .endClass();
        //.endNamespace();

    luabridge::getGlobalNamespace(luaState)
            //.beginNamespace("scene")
            .beginClass<Matrix3>("Mat3")
            .endClass();
        //.endNamespace();

    luabridge::getGlobalNamespace(luaState)
            //.beginNamespace("scene")
            .beginClass<Matrix4>("Mat4")
            .endClass();
        //.endNamespace();

    luabridge::getGlobalNamespace(luaState)
            //.beginNamespace("scene")
            .beginClass<Camera>("Camera")
            .addConstructor<void (*)(Vector3, Vector3, Vector3, Vector3)>()
            .addProperty("view_matrix", &Camera::getViewMatrix)
            .addProperty("projection_matrix", &Camera::getProjectionMatrix)
            .addFunction("rotate", &Camera::rotate)
            .endClass();
        //.endNamespace();

    // define renderer singleton
    //luabridge::getGlobalNamespace(luaState)
    //        //.beginNamespace("scene")
    //        .beginClass<Renderer>("Renderer")
    //        .addFunction("use_shader_program", &Renderer::useShaderProgram)
    //        .addFunction("use_framebuffer", &Renderer::useFramebuffer)
    //        .addFunction("set_uniform",
    //                     luabridge::overload<const std::string&, Vector2>(&Renderer::setUniform),
    //                     luabridge::overload<const std::string&, Vector3>(&Renderer::setUniform),
    //                     luabridge::overload<const std::string&, Matrix3>(&Renderer::setUniform),
    //                     luabridge::overload<const std::string&, Matrix4>(&Renderer::setUniform))
    //        .addFunction("set_texture", &Renderer::setTexture)
    //        .addFunction("draw_3d_model", &Renderer::draw3DModel)
    //        .endClass();
        //.endNamespace();

    auto renderer = std::make_unique<Renderer>();

    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("renderer")
            .addFunction("use_shader_program", [&](const std::string& name) { renderer->useShaderProgram(name); })
            .addFunction("use_framebuffer", [&](const std::string& name) { renderer->useFramebuffer(name); })
            .addFunction("set_uniform",
                         [&](const std::string& name, Vector2 value) { renderer->setUniform(name, value); },
                         [&](const std::string& name, Vector3 value) { renderer->setUniform(name, value); },
                         [&](const std::string& name, Matrix3 value) { renderer->setUniform(name, value); },
                         [&](const std::string& name, Matrix4 value) { renderer->setUniform(name, value); })
            .addFunction("set_texture", [&](const std::string& name, const std::string& resource) { renderer->setTexture(name, resource); })
            .addFunction("draw_3d_model", [&](const std::string& name, Matrix4 value) { renderer->draw3DModel(name); })
        .endNamespace();

    // define resource manager singleton
    //luabridge::getGlobalNamespace(luaState)
    //        // .beginNamespace("scene")
    //        .beginClass<ResourceManager>("ResourceManager")
    //        .addFunction("add_3d_model", &ResourceManager::add3DModel)
    //        .addFunction("add_texture", &ResourceManager::addTexture)
    //        .endClass();
    //    //.endNamespace();

    auto resources = std::make_unique<ResourceManager>();

    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("resources")
            .addFunction("add_3d_model", [&](const std::string& name, const std::string& path) { resources->add3DModel(name, path); })
            .addFunction("add_texture", [&](const std::string& name, const std::string& path) { resources->addTexture(name, path); })
        .endNamespace();

    // define input manager singleton
    //luabridge::getGlobalNamespace(luaState)
    //        //.beginNamespace("scene")
    //        .beginClass<InputManager>("InputManager")
    //        .addProperty("mouse_position", &InputManager::getMousePosition)
    //        .addFunction("is_mouse_button_pressed", &InputManager::isMouseButtonPressed)
    //        .endClass();
        //.endNamespace();

    auto inputs = std::make_unique<InputManager>();

    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("inputs")
            .addProperty("mouse_position", [&]() { return inputs->getMousePosition(); })
            .addFunction("is_mouse_button_pressed", [&](int b) { return inputs->isMouseButtonPressed(b); })
        .endNamespace();

    // define global constants: MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT
    auto MOUSE_BUTTON_LEFT = 1;
    auto MOUSE_BUTTON_RIGHT = 2;

    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("inputs")
            .addProperty("MOUSE_BUTTON_LEFT", &MOUSE_BUTTON_LEFT)
            .addProperty("MOUSE_BUTTON_RIGHT", &MOUSE_BUTTON_RIGHT)
        .endNamespace();

    // load some code from Lua file
    int scriptLoadStatus = luaL_dofile(luaState, "resources/scenes/level1.lua");

    // define error reporter for any Lua error
    report_errors(luaState, scriptLoadStatus);

    // call function defined in Lua script
    luabridge::LuaRef onLoad = luabridge::getGlobal(luaState, "on_load");
    luabridge::LuaRef onRender = luabridge::getGlobal(luaState, "on_render");

    if (onLoad.isValid())
    {
        onLoad();
    }

    if (onRender.isValid())
    {
        onRender(0.1);
    }

    return 0;
}
