#include "worm_dust.hpp"

using namespace cgp;

WormDust::WormDust() {}

WormDust::WormDust(int n_particules) {
    this->n_particules = n_particules;
    pos.resize(n_particules);
    phong.resize(n_particules);

    mesh sphere = mesh_primitive_sphere();
    particule.initialize_data_on_gpu(sphere);
    particule.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/sand.jpg", GL_REPEAT, GL_REPEAT);

    for (int i = 0; i < n_particules; i++) {
        pos[i] = {0.2 * i, 0.2 * i, 0.2 * i};
        phong[i] = {0, 0, 1, (float)i / n_particules};
    }

    particule.initialize_supplementary_data_on_gpu(pos, 4, 1);
    particule.initialize_supplementary_data_on_gpu(phong, 5, 1);
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
