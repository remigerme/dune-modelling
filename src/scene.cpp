#include "scene.hpp"

#include "terrain.hpp"
#include "worm.hpp"

using namespace cgp;

// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once
void scene_structure::initialize() {
    // Set the behavior of the camera and its initial position
    // ********************************************** //
    camera_control.initialize(inputs, window);
    camera_control.set_rotation_axis_z(); // camera rotates around z-axis
    //   look_at(camera_position, targeted_point, up_direction)
    camera_control.look_at(
        {3.0f, -2.0f, 1.5f} /* position of the camera in the 3D scene */,
        {0, 0, 0} /* targeted point in 3D scene */,
        {0, 0, 1} /* direction of the "up" vector */);

    // General information
    display_info();

    // Create the global (x,y,z) frame
    // global_frame.initialize_data_on_gpu(mesh_primitive_frame());

    // Create the shapes seen in the 3D scene
    // ********************************************** //

    // Terrain : the ground (dunes)
    float ground_size = 500; // scaling factor
    float uv_range = 3;      // used for repetition of the texture
    mesh ground_mesh = create_ground_mesh(uv_range);
    ground.initialize_data_on_gpu(ground_mesh);
    ground.model.set_scaling(ground_size);
    ground.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/sand.jpg", GL_REPEAT, GL_REPEAT);
    // Light settings - colors are handled by initialize_ground
    ground.material.phong.specular = 0;
    ground.material.phong.diffuse = 0.8f;
    ground.material.phong.ambient = 0.6f;

    // Terrain : the surrounding cliff
    mesh cliff_mesh = create_cliff_mesh();
    cliff.initialize_data_on_gpu(cliff_mesh);
    cliff.model.set_scaling(ground_size);
    cliff.material.color = {0, 1, 0};

    // Default FOV
    camera_projection.field_of_view = Pi / 2.0f;

    // Load a shader from a file
    opengl_shader_structure shader_custom;
    shader_custom.load(project::path + "shaders/mesh/mesh.vert.glsl",
                       project::path + "shaders/mesh/mesh.frag.glsl");
}

/*
This function is called permanently at every new frame
Note that you should avoid having costly computation and large allocation
defined there. This function is mostly used to call the draw() functions on
pre-existing data.
*/
void scene_structure::display_frame() {
    // Set the light to the current position of the camera
    environment.light = camera_control.camera_model.position();

    // Updating variables
    timer.update();

    // Passing uniform variables to shaders
    environment.uniform_generic.uniform_float["time"] = timer.t;

    // Drawing objects
    draw(ground, environment);
    draw(cliff, environment);
    draw(worm, environment);

    // conditional display of the global frame (set via the GUI)
    if (gui.display_frame)
        draw(global_frame, environment);
}

void scene_structure::display_gui() {
    ImGui::Checkbox("Frame", &gui.display_frame);
    ImGui::Checkbox("Wireframe", &gui.display_wireframe);
    ImGui::SliderFloat("FOV", &camera_projection.field_of_view,
                       10.0f * Pi / 180.0f, 150.0f * Pi / 180.0f);
}

void scene_structure::mouse_move_event() {
    if (!inputs.keyboard.shift)
        camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event() {
    camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event() {
    camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame() {
    camera_control.idle_frame(environment.camera_view);
}

void scene_structure::display_info() {
    std::cout << "\nCAMERA CONTROL:" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << camera_control.doc_usage() << std::endl;
    std::cout << "-----------------------------------------------\n"
              << std::endl;

    std::cout << "\nSCENE INFO:" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Introductory scene for INF443." << std::endl;
    std::cout << "-----------------------------------------------\n"
              << std::endl;
}
