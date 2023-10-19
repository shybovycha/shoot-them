require "resources/scenes/common"

function load_level()
    resources:add_3d_model("square", "resources/models/old/square1.dae")

    scene_data.targets = {
        { position = Vec3(58.497306823730469, -63.435737609863281, 473.66156005859375) },
        { position = Vec3(-33.380451202392578, -6.5831127166748047, 447.39724731445313) },
        { position = Vec3(-24.753301620483398, -8.1592473983764648, 412.58016967773438) },
        { position = Vec3(19.433996200561523, -10.429162979125977, 435.2579345703125) },
        { position = Vec3(67.2529296875, -65.851509094238281, 408.95989990234375) },
        { position = Vec3(-100.23522186279297, -96.2567138671875, 471.880615234375) },
        { position = Vec3(-149.78047180175781, -94.133956909179688, 349.2867431640625) },
        { position = Vec3(217.39193725585938, 107.91432952880859, 990.87042236328125) },
        { position = Vec3(213.81425476074219, 103.36730194091797, 978.5133056640625) },
        { position = Vec3(-202.65162658691406, 95.971572875976563, 967.23687744140625) },
        { position = Vec3(-422.13534545898438, 2.4690520763397217, 756.34063720703125) },
        { position = Vec3(-327.42922973632813, -56.237167358398438, 578.69732666015625) },
        { position = Vec3(-322.5697021484375, -56.098651885986328, 556.587646484375) },
        { position = Vec3(-2.0531110763549805, -62.731517791748047, 898.2244873046875) },
        { position = Vec3(69.057952880859375, -64.81298828125, 446.350341796875) },
    }

    scene_data.lights = {
        { position = Vec3(312.20150756835938, 74.7088623046875, 129.395263671875), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(309.84982299804688, 80.696151733398438, 743.273681640625), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(232.87030029296875, 71.250167846679688, 929.0843505859375), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-205.07649230957031, 76.857154846191406, 951.9937744140625), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-367.27157592773438, 80.252830505371094, 752.38995361328125), color = Vec4(1, 1, 1, 1), intensity = 100 },
        { position = Vec3(-380.81719970703125, 74.969322204589844, 139.01902770996094), color = Vec4(1, 1, 1, 1), intensity = 100 },
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
