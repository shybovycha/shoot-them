targets = {
    { position = Vec3(-765.06158447265625, 271.0589599609375, 670.49334716796875) },
    { position = Vec3(-48.446037292480469, 298.76113891601563, 651.969970703125) },
    { position = Vec3(746.4915771484375, 291.43478393554688, 613.66107177734375) },
}

camera = Camera(
    Vec3(0, 0, 0), -- position
    Vec3(0, 1, 0), -- up
    Vec3(0, 0, -1), -- forward
    Vec3(1, 0, 0) -- right
)

mouse_cursor_pos = inputs.mouse_position

function on_load()
    resources.add_3d_model("room", "resources/models/old/room1.dae")

    resources.add_3d_model("chicken", "resources/models/old/chicken.3ds")
    resources.add_texture("chicken_spec", "resources/models/old/Chick03.BMP")

    -- resources:add_framebuffer("rtt")
end

function on_render(delta_time)
    -- update mouse position
    current_mouse_pos = scene.inputs.mouse_position
    mouse_delta = mouse_cursor_pos - current_mouse_pos
    mouse_cursor_pos = current_mouse_pos

    camera:rotate(mouse_delta.x, mouse_delta.y)

    -- check mouse button press
    if inputs.is_mouse_button_pressed(inputs.MOUSE_BUTTON_LEFT) then
    end

    if inputs.is_mouse_button_pressed(inputs.MOUSE_BUTTON_RIGHT) then
    end

    -- render scene
    renderer.use_shader_program("default")
    renderer.use_framebuffer("default")

    renderer.set_uniform("view", camera.view_matrix)
    renderer.set_uniform("projection", camera.projection_matrix)

    renderer.set_uniform("model", Vec3(0, 0, 0))
    renderer.draw_3d_model("room")

    for target in targets do
        renderer.set_texture("specular_texture", "chicken_spec")
        renderer.set_uniform("model", target["position"])

        renderer.draw_3d_model("chicken")
    end

    -- renderer.add_light(Vec3(-717.96405029296875, 129.1875, 200), Vec3(255, 255, 255) 100.0)
    -- renderer.add_light(Vec3(0, 125.34170532226563, 258.18295288085938), Vec3(255, 255, 255) 100.0)
    -- renderer.add_light(Vec3(757.7335205078125, 194.04945373535156, 384.62527465820313), Vec3(255, 255, 255) 100.0)
end
