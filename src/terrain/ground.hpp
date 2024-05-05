#pragma once

#include "cgp/cgp.hpp"

// Initialize the mesh of the ground
// To allow the texture to repeat on a single grid
// We change (u, v) from [0,1] to [0, uv_range]
cgp::mesh create_ground_mesh(float uv_range);
