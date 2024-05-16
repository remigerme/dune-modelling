#pragma once

#include "cgp/cgp.hpp"

using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;

struct Cliff {
    vec3 cliff_scale;
    mesh cliff_mesh;
    mesh_drawable cliff_drawable;

    // Initialize the mesh of the cliff and the mesh_drawable
    // We scale the mesh_drawable by cliff_scale
    // We change (u, v) from [0,1] to [0, uv_range]
    // To allow the texture to repeat on a single grid
    Cliff(vec3 cliff_scale, float uv_range);

    // Just to avoid compilation warnings
    // Do nothing and should do nothing
    // else is called before some initialization stuff
    // in cgp and segfault yeepee
    Cliff();
};
