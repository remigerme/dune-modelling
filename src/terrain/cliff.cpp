#include "cliff.hpp"
#include "../noise/perlin.hpp"

using namespace cgp;

float compute_cliff_height(float u, float v, perlin_noise_parameters p) {
    float p_noise = noise_perlin({u, v}, p.octave, p.persistency, p.frequency);
    float u_ctrd = u - 0.5;
    float v_ctrd = v - 0.5;
    float r = sqrt(u_ctrd * u_ctrd + v_ctrd * v_ctrd);
    assert(1 - r > 0);
    std::cout << "u_ctrd : " << u_ctrd << " v_ctrd : " << v_ctrd << "    ";
    std::cout << "1 - r : " << 1 - r
              << "    z : " << exp(-10 * std::abs(0.2 - r)) << std::endl;
    return p_noise * p.height * exp(-10 * (1 - r));
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

mesh create_cliff_mesh() {
    int const cliff_samples = 300;
    mesh cliff_mesh =
        mesh_primitive_grid({-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0},
                            cliff_samples, cliff_samples);

    perlin_noise_parameters p = {0.35f, 2.5f, 3, 50};
    initialize_cliff(cliff_mesh, p);

    return cliff_mesh;
}
