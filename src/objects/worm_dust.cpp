#include "worm_dust.hpp"

using namespace cgp;

WormDust::WormDust() {}

// Function to filter upper part vertices
numarray<vec3> filter_upper_part(const numarray<vec3> positions,
                                 float threshold) {
    numarray<vec3> upper_positions;
    for (const auto &pos : positions) {
        if (pos.z > threshold) {
            upper_positions.push_back(pos);
        }
    }
    return upper_positions;
}

WormDust::WormDust(int n_particules, Worm worm) {
    this->n_particules = n_particules;
    pos.resize(n_particules);
    alpha.resize(n_particules);

    mesh sphere = mesh_primitive_sphere(3);
    particule.initialize_data_on_gpu(sphere);
    particule.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/dust.jpg", GL_REPEAT, GL_REPEAT);
    particule.material.phong = {1, 0, 0.2f, 80};

    // Filter upper part points
    float z_threshold = 0;
    numarray<vec3> upper_positions =
        filter_upper_part(worm.body_mesh.position, z_threshold);

    if (upper_positions.size() == 0) {
        throw std::runtime_error(
            "No points found in the upper part of the mesh.");
    }

    float x_offset = 10;
    int N = upper_positions.size();
    for (int i = 0; i < n_particules; ++i) {
        pos[i] = 5 * worm.scale * upper_positions[i % N];
        pos[i] -= vec3{x_offset, 0, 0};
        alpha[i] = {(float)rand() / RAND_MAX, 0};
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
