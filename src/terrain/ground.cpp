#include "ground.hpp"

#include "../noise/perlin.hpp"

using namespace cgp;

float compute_noise(float u, float v, perlin_noise_parameters parameters) {
    // Initial Perloin noise
    float p_noise = noise_perlin({u, v}, parameters.octave,
                                 parameters.persistency, parameters.frequency);

    // We rescale coordinates for custom dune profil
    float us = 7 * u - 1;
    float vs = 7 * v - 1;

    // We add dune profile
    float d = sin(us + vs + us * vs) / (1 + std::abs(us + vs));
    float noise = (p_noise + d) / 2;

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
                0.3f * vec3(1, 0.55f, 0) + 0.7f * noise * vec3(1, 0.85f, 0);
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

    // Manually set perlin noise
    perlin_noise_parameters p = {0.35f, 3.5f, 3, 0.35f};
    initialize_ground(ground_mesh, p);

    return ground_mesh;
}

Ground::Ground(vec3 ground_scale, float uv_range) {
    this->ground_scale = ground_scale;
    ground_mesh = create_ground_mesh(uv_range);
    ground_drawable.initialize_data_on_gpu(ground_mesh);
    ground_drawable.model.set_scaling_xyz(ground_scale);
    ground_drawable.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/sand.jpg", GL_REPEAT, GL_REPEAT);
    // Light settings - colors are handled by initialize_ground_drawable
    ground_drawable.material.phong.specular = 0;
    ground_drawable.material.phong.diffuse = 0;
    ground_drawable.material.phong.ambient = 1.5f;

    dust.initialize_data_on_gpu(ground_mesh);
    dust.model.set_scaling_xyz(ground_scale);
    dust.model.set_translation({0, 0, 0.1f});
    dust.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/sand.jpg", GL_REPEAT, GL_REPEAT);
    dust.material.alpha = 0.3f;
    dust.material.phong.specular = 0;
    dust.material.phong.diffuse = 0;
    dust.material.phong.ambient = 1.5f;
    opengl_shader_structure dust_shader;
    dust_shader.load(project::path + "shaders/ground/dust.vert.glsl",
                     project::path + "shaders/mesh/mesh.frag.glsl"); // default
    dust.shader = dust_shader;
}

Ground::Ground() {}

void Ground::display(environment_structure environment) {
    draw(ground_drawable, environment);

    // Enable use of alpha component as color blending for transparent elements
    //  alpha = current_color.alpha
    //  new color = previous_color * alpha + current_color * (1-alpha)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);

    draw(dust, environment);

    // Don't forget to re-activate the depth-buffer write
    glDepthMask(true);
    glDisable(GL_BLEND);
}
