#include "cliff.hpp"

#include "../noise/perlin.hpp"
#include "environment.hpp"

using namespace cgp;

float compute_cliff_height(float u, float v, perlin_noise_parameters p) {
    float p_noise = noise_perlin({u, v}, p.octave, p.persistency, p.frequency);
    float u_ctrd = u - 0.5;
    float v_ctrd = v - 0.5;
    float r = sqrt(u_ctrd * u_ctrd + v_ctrd * v_ctrd);
    float r_max = sqrt(0.5 * 0.5);

    // Affine par morceaux
    float epsilon = 0.1;
    float d = 0;

    if (std::abs(r_max - r) > epsilon)
        return 0;

    d = std::min(1.0f, (r - (r_max - epsilon)) / epsilon);

    return (4 * p_noise + 3 * d) / 7;
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

            // The higher the darker
            vec3 dark = {60.0f / 255, 40.0f / 255, 20.0f / 255};
            dark = {0, 1, 0};
            vec3 light = {150.0f / 255, 100.0f / 255, 60.0f / 255};
            light = {1, 0, 0};

            float lambda = 7 * height / (4 + 3 * 2);

            std::cout << lambda << std::endl;

            cliff.color[idx] = (1 - lambda) * light + lambda * dark;
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

Cliff::Cliff(vec3 cliff_scale, float uv_range) {
    this->cliff_scale = cliff_scale;
    cliff_mesh = create_cliff_mesh(uv_range);
    cliff_drawable.initialize_data_on_gpu(cliff_mesh);
    cliff_drawable.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/cliff_light.jpg", GL_REPEAT, GL_REPEAT);
    cliff_drawable.model.set_scaling_xyz(cliff_scale);

    // Light settings - colors are handled by initialize_cliff_drawable
    cliff_drawable.material.phong.specular = 0;
    cliff_drawable.material.phong.diffuse = 0;
    cliff_drawable.material.phong.ambient = 1.5f;
}

Cliff::Cliff() {}
