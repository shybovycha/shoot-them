require "resources/scenes/common"

function load_level()
    resources:add_3d_model("forest", "resources/models/old/forest1.dae")

    scene_data.targets = {
        { position = Vec3(501.74432373046875, 367.27285766601563, 177.75527954101563), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-283.0126953125, 370.33697509765625, 388.32470703125), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-522.38800048828125, 363.69537353515625, -121.65966796875), color = Vec4 (1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-75.586532592773438, 352.65716552734375, -810.167724609375), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(816.7041015625, 335.552734375, -395.20077514648438), color = Vec4(1, 1, 1, 1), intensity = 100 },
    }

    scene_data.lights = {
        { position = Vec3(-14.414848327636719, 189.5013427734375, 481.59619140625), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(7.109917163848877, 101.456298828125, -254.32777404785156), color = Vec4(1, 1, 1, 1), intensity = 100 },
    }
end

function render_level(delta_time)
    renderer:set_uniform("model", Vec3(0, 0, 0))
    renderer:draw_3d_model("forest")
end

function on_load()
    load_common_scene(load_level)
end

function on_render(delta_time)
    render_common_scene(delta_time, render_level)
end
