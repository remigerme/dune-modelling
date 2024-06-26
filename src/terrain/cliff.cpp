#include "cliff.hpp"

#include "../noise/perlin.hpp"
#include "environment_dune.hpp"

using namespace cgp;

float compute_cliff_height(float u, float v, perlin_noise_parameters p) {
    float p_noise = noise_perlin({u, v}, p.octave, p.persistency, p.frequency);
    float u_ctrd = 2 * (u - 0.5);
    float v_ctrd = 2 * (v - 0.5);
    float d = std::min(1.0, pow(pow(u_ctrd, 2) + pow(v_ctrd, 2), 3));
    return 0.4 * p_noise + 0.55 * d;
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

            float height = compute_cliff_height(u, v, parameters);
            cliff.position[idx].z = height;

            if (height == 0)
                continue;

            // The higher the lighter
            vec3 dark = {120.0f / 255, 70.0f / 255, 30.0f / 255};
            dark = {0, 0, 0};
            vec3 light = {240.0f / 255, 140.0f / 255, 50.0f / 255};

            float lambda = height;

            cliff.color[idx] = (1 - lambda) * dark + lambda * light;
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

    perlin_noise_parameters p = {0.35f, 2.5f, 4, 1};
    initialize_cliff(cliff_mesh, p);

    return cliff_mesh;
}

Cliff::Cliff(vec3 cliff_scale, float uv_range) {
    this->cliff_scale = cliff_scale;
    cliff_mesh = create_cliff_mesh(uv_range);
    cliff_drawable.initialize_data_on_gpu(cliff_mesh);
    cliff_drawable.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/cliff.jpg", GL_REPEAT, GL_REPEAT);
    cliff_drawable.model.set_scaling_xyz(cliff_scale);

    // Light settings - colors are handled by initialize_cliff_drawable
    cliff_drawable.material.phong.specular = 0;
    cliff_drawable.material.phong.diffuse = 0;
    cliff_drawable.material.phong.ambient = 1.0f;
}

Cliff::Cliff() {}
