require "resources/scenes/common"

function load_level()
    resources:add_3d_model("temple", "resources/models/old/egypt1.dae")

    scene_data.targets = {
        { position = Vec3(133.86979675292969, 26.722480773925781, 752.10870361328125) },
        { position = Vec3(-1.9705199003219604, 97.372062683105469, 781.0562744140625) },
        { position = Vec3(135.13320922851563, -17.226655960083008, 562.41522216796875) },
        { position = Vec3(-210.22236633300781, -19.128067016601563, 494.866455078125) },
        { position = Vec3(-103.61713409423828, 30.607658386230469, 751.9798583984375) },
        { position = Vec3(-4.7911362648010254, 65.196853637695313, 895.4205322265625) },
    }

    scene_data.lights = {
        { position = Vec3(283.33493041992188, 105.99095153808594, 454.46566772460938), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(202.56625366210938, 93.208389282226563, 789.3785400390625), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(297.8533935546875, 101.46237945556641, 961.899169921875), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-310.04443359375, 97.293807983398438, 776.50921630859375), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-215.06777954101563, 103.75644683837891, 774.8204345703125), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-290.11581420898438, 100.30194091796875, 448.59164428710938), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(290.63540649414063, 104.52780151367188, 160.19342041015625), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-304.854248046875, 101.65565490722656, 158.61602783203125), color = Vec4(1, 1, 1, 1), intensity = 100 },
    }
end

function render_level(delta_time)
    renderer:set_uniform("model", Vec3(0, 0, 0))
    renderer:draw_3d_model("temple")
end

function on_load()
    load_common_scene(load_level)
end

function on_render(delta_time)
    render_common_scene(delta_time, render_level)
end
