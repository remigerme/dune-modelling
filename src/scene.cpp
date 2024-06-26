#include "scene.hpp"
#include <cmath>

using namespace cgp;

float dist(vec2 a, vec2 b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
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
    // Unfortunately no time to fix the shader
    // ground.ground_drawable.shader.load(
    //     project::path + "shaders/ground/thumper.vert.glsl",
    //    project::path + "shaders/mesh/mesh.frag.glsl");

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

    float dist_eps = 0.5f;

    float xm = marteleur_xy[0];
    float ym = marteleur_xy[1];

    if (marteleur_status) {
        worm.is_idling = false;
        // if we are close the thumper
        if (dist(worm.position, vec2{xm, ym}) < dist_eps) {
            marteleur_status = false;
            wtarget = worm.idle(0, ground, 1); // set target to fixed point
        } else {
            wtarget = {xm, ym};
        }
    } else {
        // if we are close worm target (ie fixed point on idle)
        if (dist(worm.position, wtarget) < dist_eps) {
            worm.is_idling = true;
        }
    }

    if (!worm.is_idling)
        idle_timer_beginning = timer.t;

    // Updating worm
    vec3 pw;
    float aw = 0;
    if (worm.is_idling) {
        float idle_timer = timer.t - idle_timer_beginning;
        vec3 posw = worm.idle(idle_timer, ground, 0.1f);
        aw = posw.z;
        pw = {posw.x, posw.y, ground.get_height(posw.x, posw.y)};
    } else {
        vec3 pw_ = worm.worm_position_orientation(3, wtarget);
        aw = pw_.z;
        pw = {pw_.x, pw_.y, ground.get_height(pw_.x, pw_.y)};
    }

    worm.set_position(pw.x, pw.y, ground);
    worm.worm["worm_body"].transform_local.set_rotation(
        rotation_transform::from_axis_angle({0, 0, 1}, aw));
    worm.worm.update_local_to_global_coordinates();

    // Updating thumper
    marteleur.marteleur["haut"].transform_local.translation = {
        0, 0, 0.85 + std::max(0.15, 0.4 * cos(timer.t) * cos(timer.t))};
    marteleur.set_position(marteleur_xy, ground);
    marteleur.marteleur.update_local_to_global_coordinates();

    // Passing uniform variables to shaders
    environment.uniform_generic.uniform_float["time"] = timer.t;
    environment.uniform_generic.uniform_float["xm"] = xm;
    environment.uniform_generic.uniform_float["ym"] = ym;
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
