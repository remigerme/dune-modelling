#include "scene.hpp"

using namespace cgp;

float max(float a, float b) {
    if (a < b)
        return b;
    else
        return a;
}

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
    vec3 ground_scale = 200 * vec3{1, 1, 1}; // scaling factor
    float ground_uv_range = 3; // used for repetition of the texture
    ground = Ground(ground_scale, ground_uv_range);

    // Terrain : the surrounding cliff
    vec3 cliff_scale = vec3{200, 200, 250}; // scaling factor
    float cliff_uv_range = 30; // used for repetition of the texture
    cliff = Cliff(cliff_scale, cliff_uv_range);
    // To be sure to be below the dunes
    cliff.cliff_drawable.model.set_translation({0, 0, -100});

    // Default FOV
    camera_projection.field_of_view = Pi / 2.0f;

    // Load a shader from a file
    opengl_shader_structure shader_marteleur;
    shader_marteleur.load(
        project::path + "shaders/shading_marteleur/shading_marteleur.vert.glsl",
        project::path + "shaders/mesh/mesh.frag.glsl");
    // ground.shader = shader_marteleur;

    marteleur = Marteleur(1);
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
    float X = 0;
    float Y = 0;

    marteleur.marteleur["haut"].transform_local.translation = {
        0, 0, 0.85 + max(0.15, 0.4 * cos(timer.t) * cos(timer.t))};
    // float hauteur = evaluate_terrain_height(0, 0);
    marteleur.marteleur["tube_central"].transform_local.translation = {
        X, Y, 0}; // en vrai, mettre terrain_height

    // Passing uniform variables to shaders
    marteleur.marteleur.update_local_to_global_coordinates();
    environment.uniform_generic.uniform_float["time"] = timer.t;
    environment.uniform_generic.uniform_float["X"] = X;
    environment.uniform_generic.uniform_float["Y"] = Y;

    // Drawing objects
    draw(ground.ground_drawable, environment);
    draw(cliff.cliff_drawable, environment);
    draw(worm, environment);
    draw(marteleur.marteleur, environment);

    // conditional display of the global frame (set via the GUI)
    if (gui.display_frame)
        draw(global_frame, environment);
}

void scene_structure::display_gui() {
    ImGui::Checkbox("Frame", &gui.display_frame);
    ImGui::Checkbox("Wireframe", &gui.display_wireframe);
    ImGui::SliderFloat("Ambient", &cliff.cliff_drawable.material.phong.ambient,
                       0, 5);
    ImGui::SliderFloat("Diffuse", &cliff.cliff_drawable.material.phong.diffuse,
                       0, 5);
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
