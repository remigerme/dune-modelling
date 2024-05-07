#include "worm.hpp"

using namespace cgp;

cgp::mesh create_worm(float radius, float length) {
    // Shortcuts variables
    float x_min = -length / 2;
    float x_max = length / 2;

    // The pointy tail
    mesh tail =
        mesh_primitive_cone(radius, length / 4, {x_min, 0, 0}, {-1, 0, 0});

    // The body
    vec3 p1 = {x_min, 0, 0};
    vec3 p2 = {x_max, 0, 0};
    mesh body = mesh_primitive_cylinder(radius, p1, p2);

    // The mouth and the teeth
    mesh mouth = mesh_primitive_disc(radius, {x_max, 0, 0}, {1, 0, 0});

    mesh worm = body;
    worm.push_back(tail);
    worm.push_back(mouth);
    return worm;
}