#include "ground.hpp"
#include "../noise/perlin.hpp"

using namespace cgp;

float compute_noise(float u, float v, perlin_noise_parameters parameters) {
    // Initial Perloin noise
    float p_noise = noise_perlin({u, v}, parameters.octave,
                                 parameters.persistency, parameters.frequency);

    // We add dune profile
    float d = sin(4 * M_PI * (u + v)) / 3;
    float noise = p_noise * d;

    return noise;
}

void initialize_ground(mesh &ground, perlin_noise_parameters parameters) {
    // Number of samples in each direction (assuming a square grid)
    int const N = std::sqrt(ground.position.size());

    // Computes the vertices
    for (int ku = 0; ku < N; ++ku) {
        for (int kv = 0; kv < N; ++kv) {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            int const idx = ku * N + kv;

            const float noise = compute_noise(u, v, parameters);

            // use the noise as height value
            ground.position[idx].z = parameters.height * noise;

            // use also the noise as color value
            ground.color[idx] =
                0.2f * vec3(1, 0.6f, 0) + 0.8f * noise * vec3(1, 0.8f, 0);
        }
    }
}

mesh create_ground_mesh(float uv_range) {
    int const ground_sample = 300;
    mesh ground_mesh =
        mesh_primitive_grid({-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0},
                            ground_sample, ground_sample);

    // To allow the texture to repeat on a single grid
    // We change (u, v) from [0,1] to [0, uv_range]
    for (int ku = 0; ku < ground_sample; ku++) {
        for (int kv = 0; kv < ground_sample; kv++) {
            int const idx = ku * ground_sample + kv;
            float u = ku / (ground_sample - 1.0f);
            float v = kv / (ground_sample - 1.0f);
            u *= uv_range;
            v *= uv_range;
            ground_mesh.uv[idx] = {u, v};
        }
    }

    perlin_noise_parameters p = {0.35f, 3.5f, 3, 0.35f};
    initialize_ground(ground_mesh, p);

    return ground_mesh;
}
