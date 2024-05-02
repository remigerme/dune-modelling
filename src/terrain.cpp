#include "terrain.hpp"

using namespace cgp;

float compute_noise(float u, float v, perlin_noise_parameters parameters) {
    // Initial Perloin noise
    float p_noise =
        noise_perlin({u, v}, parameters.octave, parameters.persistency,
                     parameters.frequency_gain);
    return p_noise;
}

void initialize_terrain(mesh &terrain, perlin_noise_parameters parameters) {
    // Number of samples in each direction (assuming a square grid)
    int const N = std::sqrt(terrain.position.size());

    // Computes the vertices
    for (int ku = 0; ku < N; ++ku) {
        for (int kv = 0; kv < N; ++kv) {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            int const idx = ku * N + kv;

            const float noise = compute_noise(u, v, parameters);

            // use the noise as height value
            terrain.position[idx].z = parameters.terrain_height * noise;

            // use also the noise as color value
            terrain.color[idx] =
                0.2f * vec3(1, 0.6f, 0) + 0.8f * noise * vec3(1, 0.8f, 0);
        }
    }
}

mesh create_terrain_mesh(float uv_range) {
    int const terrain_sample = 300;
    mesh terrain_mesh =
        mesh_primitive_grid({-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0},
                            terrain_sample, terrain_sample);

    // To allow the texture to repeat on a single grid
    // We change (u, v) from [0,1] to [0, uv_range]
    for (int ku = 0; ku < terrain_sample; ku++) {
        for (int kv = 0; kv < terrain_sample; kv++) {
            int const idx = ku * terrain_sample + kv;
            float u = ku / (terrain_sample - 1.0f);
            float v = kv / (terrain_sample - 1.0f);
            u *= uv_range;
            v *= uv_range;
            terrain_mesh.uv[idx] = {u, v};
        }
    }

    initialize_terrain(terrain_mesh, perlin_noise_parameters());

    return terrain_mesh;
}
