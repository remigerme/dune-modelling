#include "scene.hpp"

#include "marteleur.hpp"
#include "terrain.hpp"
#include "worm.hpp"

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
    float ground_size = 50; // scaling factor
    float uv_range = 3;     // used for repetition of the texture
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
    opengl_shader_structure shader_marteleur;
    shader_marteleur.load(
        project::path + "shaders/shading_marteleur/shading_marteleur.vert.glsl",
        project::path + "shaders/mesh/mesh.frag.glsl");
    ground.shader = shader_marteleur;

    // Create the hierarchy
    // ************************************ //

    // Initialize the temporary mesh_drawable that will be inserted in the
    // hierarchy
    mesh_drawable tube_central;
    mesh_drawable corrole;
    mesh_drawable disc1;
    mesh_drawable disc2;
    mesh_drawable bas;
    mesh_drawable haut;
    mesh_drawable tete;
    mesh_drawable cone_bas;
    mesh_drawable pas_de_vis;
    mesh_drawable disc3;
    mesh_drawable pas_bas;

    // Create the geometry of the meshes

    tube_central.initialize_data_on_gpu(
        mesh_primitive_cylinder(0.05f, {0, 0, 0}, {0, 0, 1.5}));
    corrole.initialize_data_on_gpu(
        mesh_primitive_cylinder(0.3f, {0, 0, 0}, {0, 0, 0.05}));
    disc1.initialize_data_on_gpu(mesh_primitive_disc(0.3f));
    disc2.initialize_data_on_gpu(mesh_primitive_disc(0.3f));
    bas.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.21, 0, 0.25, 0.25));
    haut.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.25, 0, 0.21, 0.55));
    tete.initialize_data_on_gpu(
        mesh_primitive_ellipsoid({0.21, 0.21, 0.21}, {0, 0, 0}));
    cone_bas.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.15, 0, 0.21, 0.1));
    pas_de_vis.initialize_data_on_gpu(
        mesh_primitive_cylinder(0.15, {0, 0, 0}, {0, 0, 0.15}));
    disc3.initialize_data_on_gpu(mesh_primitive_disc(0.25f));
    pas_bas.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.05, 0, 0.15, 0.05));

    // Set the color of some elements
    haut.material.color = {1.0f, 0.5f, 0.1f};

    // Add the elements in the hierarchy

    marteleur.add(tube_central, "tube_central");
    marteleur.add(corrole, "corrole", "tube_central", {0, 0, 1});
    marteleur.add(disc1, "disc1", "corrole", {0, 0, 0.05});
    marteleur.add(disc2, "disc2", "corrole", {0, 0, 0});
    marteleur.add(bas, "bas", "tube_central", {0, 0, 0.75});
    marteleur.add(haut, "haut", "tube_central", {0, 0, 1});
    marteleur.add(tete, "tete", "haut", {0, 0, 0.55});
    marteleur.add(disc3, "disc3", "haut", {0, 0, 0});
    marteleur.add(cone_bas, "cone_bas", "tube_central", {0, 0, 0.65});
    marteleur.add(pas_de_vis, "pas_de_vis", "tube_central", {0, 0, 0.50});
    marteleur.add(pas_bas, "pas_bas", "tube_central", {0, 0, 0.45});
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

    marteleur["haut"].transform_local.translation = {
        0, 0, 0.85 + max(0.15, 0.4 * cos(timer.t) * cos(timer.t))};
    // float hauteur = evaluate_terrain_height(0, 0);
    marteleur["tube_central"].transform_local.translation = {
        X, Y, 0}; // en vrai, mettre terrain_height

    // Passing uniform variables to shaders
    marteleur.update_local_to_global_coordinates();
    environment.uniform_generic.uniform_float["time"] = timer.t;
    environment.uniform_generic.uniform_float["X"] = X;
    environment.uniform_generic.uniform_float["Y"] = Y;

    // Drawing objects
    draw(ground, environment);
    draw(cliff, environment);
    draw(worm, environment);
    draw(marteleur, environment);

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
