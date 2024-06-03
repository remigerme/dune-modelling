#pragma once

#include "../noise/perlin.hpp"
#include "cgp/cgp.hpp"
#include "environment_dune.hpp"

using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;

struct Ground {
    vec3 ground_scale;
    mesh ground_mesh;
    mesh_drawable ground_drawable;
    mesh_drawable dust;

    // Manually set perlin noise
    perlin_noise_parameters perlin = {0.35f, 3.5f, 3, 0.35f};

    // Initialize the mesh of the ground and the mesh_drawable
    // We scale the mesh_drawable by ground_scale
    // We change (u, v) from [0,1] to [0, uv_range]
    // To allow the texture to repeat on a single grid
    Ground(vec3 ground_scale, float uv_range);

    // Just to avoid compilation warnings
    // Do nothing and should do nothing
    // else is called before some initialization stuff
    // in cgp and segfault yeepee
    Ground();

    // Compute scaled height at scaled coordinates (x, y) by default
    // Coordinates are in [-scale.x, scale.x]*[-scale.y, scale.y]
    // We can pass unscaled coordinates too (x, y) in [0, 1]*[0, 1]
    float get_height(float x, float y, bool scaled = false);

    void display(environment_structure environement);
};
