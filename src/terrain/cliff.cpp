#include "cliff.hpp"
#include "../noise/perlin.hpp"

using namespace cgp;

float compute_cliff_height(float u, float v, perlin_noise_parameters p) {
    float p_noise = noise_perlin({u, v}, p.octave, p.persistency, p.frequency);
    float u_ctrd = u - 0.5;
    float v_ctrd = v - 0.5;
    float r = sqrt(u_ctrd * u_ctrd + v_ctrd * v_ctrd);
    return p_noise * p.height * exp(-5 * (0.4 - r));
}

void initialize_cliff(mesh &cliff, perlin_noise_parameters parameters) {
    // Number of samples in each direction (assuming a square grid)
    int const N = std::sqrt(cliff.position.size());

    // Computes the vertices
    for (int ku = 0; ku < N; ++ku) {
        for (int kv = 0; kv < N; ++kv) {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            int const idx = ku * N + kv;

            cliff.position[idx].z = compute_cliff_height(u, v, parameters);
        }
    }
}

mesh create_cliff_mesh(float uv_range) {
    int const cliff_samples = 300;
    mesh cliff_mesh =
        mesh_primitive_grid({-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0},
                            cliff_samples, cliff_samples);

    // To allow the texture to repeat on a single grid
    // We change (u, v) from [0,1] to [0, uv_range]
    for (int ku = 0; ku < cliff_samples; ku++) {
        for (int kv = 0; kv < cliff_samples; kv++) {
            int const idx = ku * cliff_samples + kv;
            float u = ku / (cliff_samples - 1.0f);
            float v = kv / (cliff_samples - 1.0f);
            u *= uv_range;
            v *= uv_range;
            cliff_mesh.uv[idx] = {u, v};
        }
    }

    perlin_noise_parameters p = {0.35f, 2.5f, 3, 1};
    initialize_cliff(cliff_mesh, p);

    return cliff_mesh;
}
