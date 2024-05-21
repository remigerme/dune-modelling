#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"

using cgp::camera_controller_orbit_euler;
using cgp::camera_projection_perspective;
using cgp::mesh_drawable;

struct WormDust {
    std::vector<mesh_drawable> quads;

    WormDust(int n_quads);

    void display(environment_structure environment,
                 camera_controller_orbit_euler camera_control,
                 camera_projection_perspective camera_projection);
};