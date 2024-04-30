#include "scene.hpp"

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
    global_frame.initialize_data_on_gpu(mesh_primitive_frame());

    // Create the shapes seen in the 3D scene
    // ********************************************** //

    // Defining the ground which is a quadrangle
    float L_ground = 3.0f;
    float z_ground = -0.51f;
    mesh ground_mesh = mesh_primitive_quadrangle(
        {-L_ground, -L_ground, z_ground}, {L_ground, -L_ground, z_ground},
        {L_ground, L_ground, z_ground}, {-L_ground, L_ground, z_ground});
    ground.initialize_data_on_gpu(ground_mesh);

    // Default FOV
    camera_projection.field_of_view = Pi / 2.0f;

    // Load a shader from a file
    opengl_shader_structure shader_custom;
    shader_custom.load(project::path + "shaders/mesh/mesh.vert.glsl",
                       project::path + "shaders/mesh/mesh.frag.glsl");
}

// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation
// defined there. This function is mostly used to call the draw() functions on
// pre-existing data.
void scene_structure::display_frame() {
    // Messing with the Z-Buffer
    // glDisable(GL_DEPTH_TEST);

    // Set the light to the current position of the camera
    environment.light = camera_control.camera_model.position();

    // Update time
    timer.update();

    // Passing uniform variables to shaders
    environment.uniform_generic.uniform_float["time"] = timer.t;

    // Drawing objects
    draw(ground, environment);

    // Drawing wireframes
    if (gui.display_wireframe) {
        draw_wireframe(ground, environment);
    }

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
