#include "scene.hpp"
#include <cmath>

using namespace cgp;

vec3 worm_position_orientation(float v, float time, vec2 coor, vec2 cible) {
    // on rend dans l'ordre la nouvelle position selon x, la nouvelle position
    // selon y , et l'angle d'orientation du ver
    float dt = 0.1;
    float distance_mvt = dt * v;
    float x0 = cible[0] - coor[0];
    float y0 = cible[1] - coor[1];
    float distance = sqrt(x0 * x0 + y0 * y0);
    float dX = ((rand() % 100) - 50) / 100.0f;
    float dY = ((rand() % 100) - 50) / 100.0f;

    vec2 direction = {x0 + dX * distance / 10, y0 + dY * distance / 10};
    float d = sqrt(direction[0] * direction[0] + direction[1] * direction[1]);
    float coeff_homo = distance_mvt / d;
    float alpha = atan(direction[1] / direction[0]);

    vec3 new_pos = {coor[0] + coeff_homo * direction[0],
                    coor[1] + coeff_homo * direction[1], alpha};

    return new_pos;
}

void scene_structure::initialize() {
    // Camera control
    camera_control.initialize(inputs, window);
    camera_control.look_at(
        {10, 10, 30} /* position of the camera in the 3D scene */,
        {0, 0, 30} /* targeted point in 3D scene */,
        {0, 0, 1} /* direction of the "up" vector */);
    // Default parameters value
    camera_control.translation_speed = 10;
    camera_projection.field_of_view = Pi / 2.0f;

    // General information
    display_info();

    // Terrain : the ground (dunes)
    vec3 ground_scale = 200 * vec3{1, 1, 1}; // scaling factor
    float ground_uv_range = 3; // used for repetition of the texture
    ground = Ground(ground_scale, ground_uv_range);
    // Load a shader from a file
    ground.ground_drawable.shader.load(
        project::path + "shaders/ground/thumper.vert.glsl",
        project::path + "shaders/mesh/mesh.frag.glsl");

    // Terrain : the surrounding cliff
    vec3 cliff_scale = vec3{200, 200, 250}; // scaling factor
    float cliff_uv_range = 4; // used for repetition of the texture
    cliff = Cliff(cliff_scale, cliff_uv_range);
    // To be sure to be below the dunes
    cliff.cliff_drawable.model.set_translation({0, 0, -100});

    marteleur = Marteleur(1);
    float xy[] = {0, 0};
    marteleur.set_position(xy, ground);

    float worm_scale = 7.0f;
    worm = Worm(worm_scale);

    int nb_dust_particles = 1000;
    worm_dust = WormDust(nb_dust_particles, worm);
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

    // Updating thumper
    marteleur.marteleur["haut"].transform_local.translation = {
        0, 0, 0.85 + std::max(0.15, 0.4 * cos(timer.t) * cos(timer.t))};
    marteleur.set_position(marteleur_xy, ground);
    marteleur.marteleur.update_local_to_global_coordinates();

    // Updating worm
    vec3 pw_ =
        worm_position_orientation(1, timer.t, worm.position, vec2{100, 100});
    float aw = pw_.z;
    vec3 pw = {pw_.x, pw_.y, ground.get_height(pw_.x, pw_.y)};
    worm.set_position(pw.x, pw.y, ground);
    worm.worm["worm_body"].transform_local.set_rotation(
        rotation_transform::from_axis_angle({0, 0, 1}, aw));
    worm.worm.update_local_to_global_coordinates();

    // Passing uniform variables to shaders
    environment.uniform_generic.uniform_float["time"] = timer.t;
    environment.uniform_generic.uniform_float["xm"] = marteleur_xy[0];
    environment.uniform_generic.uniform_float["ym"] = marteleur_xy[1];
    environment.uniform_generic.uniform_float["statusm"] = marteleur_status;
    environment.uniform_generic.uniform_vec3["pos_worm"] = pw;
    environment.uniform_generic.uniform_float["angle_worm"] = aw;

    // Drawing objects
    ground.display(environment);
    draw(cliff.cliff_drawable, environment);
    draw(worm.worm, environment);
    worm_dust.display(environment);
    if (marteleur_status)
        draw(marteleur.marteleur, environment);
}

void scene_structure::display_gui() {
    ImGui::Checkbox("Enable thumper", &marteleur_status);
    // We need ground_scale.x == ground_scale.y
    float s = ground.ground_scale.x;
    ImGui::SliderFloat2("Thumper position (x, y)", marteleur_xy, -s, s);
    ImGui::SliderFloat("FOV", &camera_projection.field_of_view,
                       10.0f * Pi / 180.0f, 150.0f * Pi / 180.0f);
    ImGui::SliderFloat("Translation speed", &camera_control.translation_speed,
                       1, 100);
    ImGui::SliderFloat("Rotation speed", &camera_control.rotation_speed, 0.2f,
                       10);
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
    std::cout << "------------------------------------------" << std::endl;
    std::cout << camera_control.doc_usage() << std::endl;
    std::cout << "------------------------------------------\n" << std::endl;

    std::cout << "\nSCENE INFO:" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Dune modelling project for INF443." << std::endl;
    std::cout << "------------------------------------------\n" << std::endl;
}
