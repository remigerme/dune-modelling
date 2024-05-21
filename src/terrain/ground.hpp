#pragma once

#include "cgp/cgp.hpp"

using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;

struct Ground {
    vec3 ground_scale;
    mesh ground_mesh;
    mesh_drawable ground_drawable;
    mesh_drawable dust;

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
};
