#pragma once

#include "cgp/cgp.hpp"

struct perlin_noise_parameters {
    float persistency = 0.35f;
    float frequency_gain = 2.5f;
    int octave = 3;
    float terrain_height = 0.35f;
};

// Initialize the mesh of the terrain
// To allow the texture to repeat on a single grid
// We change (u, v) from [0,1] to [0, uv_range]
cgp::mesh create_terrain_mesh(float uv_range);
