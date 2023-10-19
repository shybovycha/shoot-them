require "resources/scenes/common"

function load_level()
    resources:add_3d_model("room", "resources/models/old/room1.dae")

    scene_data.targets = {
        { position = Vec3(-765.06158447265625, 271.0589599609375, 670.49334716796875) },
        { position = Vec3(-48.446037292480469, 298.76113891601563, 651.969970703125) },
        { position = Vec3(746.4915771484375, 291.43478393554688, 613.66107177734375) },
    }

    scene_data.lights = {
        { position = Vec3(-717.96405029296875, 129.1875, 200), color = Vec4(1, 1, 1, 1), intensity = 100.0 },
        { position = Vec3(0, 125.34170532226563, 258.18295288085938), color = Vec4(1, 1, 1, 1), intensity = 100.0 },
        { position = Vec3(757.7335205078125, 194.04945373535156, 384.62527465820313), color = Vec4(1, 1, 1, 1), intensity = 100.0 },
    }
end

function render_level(delta_time)
    renderer:set_uniform("model", Vec3(0, 0, 0))
    renderer:draw_3d_model("room")
end

function on_load()
    load_common_scene(load_level)
end

function on_render(delta_time)
    render_common_scene(delta_time, render_level)
end
