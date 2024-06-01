#include "worm_dust.hpp"

using namespace cgp;

WormDust::WormDust() {}

WormDust::WormDust(int n_particules, mesh worm) {
    this->n_particules = n_particules;
    pos.resize(n_particules);
    alpha.resize(n_particules);

    mesh sphere = mesh_primitive_sphere();
    particule.initialize_data_on_gpu(sphere);
    particule.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/dust.jpg", GL_REPEAT, GL_REPEAT);
    particule.material.phong = {1, 0, 0.2f, 80};
    double phi = (1.0 + std::sqrt(5.0)) / 2.0; // golden ratio
    for (int i = 0; i < n_particules; ++i) {
        double r = 10 * std::cbrt(i + 0.5) /
                   std::cbrt(n_particules); // scale radius for uniform
                                            // distribution in volume
        double theta = 2 * M_PI * i / phi;
        double z = 1 - (2.0 * i + 1.0) / n_particules;
        double radius = std::sqrt(1 - z * z);

        pos[i].x = radius * std::cos(theta) * r;
        pos[i].y = radius * std::sin(theta) * r;
        pos[i].z = z * r;

        alpha[i] = {(float)i / n_particules, 0};
    }

    particule.initialize_supplementary_data_on_gpu(pos, 4, 1);
    particule.initialize_supplementary_data_on_gpu(alpha, 5, 1);
    particule.shader.load(project::path + "shaders/worm/dust.vert.glsl",
                          project::path + "shaders/worm/dust.frag.glsl");
}

void WormDust::display(environment_structure environment) {
    // Enable use of alpha component as color blending for transparent elements
    //  alpha = current_color.alpha
    //  new color = previous_color * alpha + current_color * (1-alpha)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);

    // Display the quads relative to their depth
    draw(particule, environment, n_particules);

    // Don't forget to re-activate the depth-buffer write
    glDepthMask(true);
    glDisable(GL_BLEND);
}
