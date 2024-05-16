#pragma once

#include "cgp/cgp.hpp"

struct Ground {
    cgp::mesh ground_mesh;
    cgp::mesh_drawable ground_drawable;

    // Initialize the mesh of the ground and the mesh_drawable
    // We scale the mesh_drawable by ground_size
    // We change (u, v) from [0,1] to [0, uv_range]
    // To allow the texture to repeat on a single grid
    Ground(float ground_size, float uv_range);

    // Ctor with default values (1, 1)
    Ground();
};
