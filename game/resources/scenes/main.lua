function load_common_models()
    resources:add_3d_model("chicken", "resources/models/old/chicken.3ds")
    resources:add_texture("chicken_spec", "resources/models/old/Chick03.BMP")

    -- resources:add_framebuffer("rtt")
end

function init_camera()
    scene_data.camera = Camera(
        Vec3(0, 0, 0), -- position
        Vec3(0, 1, 0), -- up
        Vec3(0, 0, -1), -- forward
        Vec3(1, 0, 0) -- right
    )

    scene_data.mouse_cursor_pos = inputs.mouse_position
end

function update_camera(delta_time)
    -- update mouse position
    current_mouse_pos = inputs.mouse_position
    mouse_delta = scene_data.mouse_cursor_pos - current_mouse_pos
    scene_data.mouse_cursor_pos = current_mouse_pos

    scene_data.camera:rotate(mouse_delta.x, mouse_delta.y)
end

function process_inputs()
    -- check mouse button press
    if inputs:is_mouse_button_pressed(MOUSE_BUTTON_LEFT) then
    end

    if inputs:is_mouse_button_pressed(MOUSE_BUTTON_RIGHT) then
    end
end

function render_targets(delta_time)
    -- render scene
    renderer:use_shader_program("default")
    renderer:use_framebuffer("default")

    renderer:set_uniform("view", scene_data.camera.view_matrix)
    renderer:set_uniform("projection", scene_data.camera.projection_matrix)

    for target in scene_data.targets do
        renderer:set_texture("specular_texture", "chicken_spec")
        renderer:set_uniform("model", target["position"])

        renderer:draw_3d_model("chicken")
    end
end

current_level = nil

function on_load()
    load_common_models()

    current_level = scenes:load_scene("./resources/scenes/level1.lua")
    current_level.on_load()

    init_camera()
end

function on_render(delta_time)
    update_camera(delta_time)

    process_inputs()

    renderer:use_shader_program("default")
    renderer:use_framebuffer("default")

    renderer:set_uniform("view", scene_data.camera.view_matrix)
    renderer:set_uniform("projection", scene_data.camera.projection_matrix)

    if render_level_fn then
        current_level.on_render(delta_time)
    end

    render_targets(delta_time)
end
