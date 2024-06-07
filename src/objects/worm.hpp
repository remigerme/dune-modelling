#pragma once

#include "../terrain/ground.hpp"
#include "cgp/cgp.hpp"

using cgp::hierarchy_mesh_drawable;
using cgp::mesh;
using cgp::vec2;
using cgp::vec3;

struct Worm {
    // DO NOT USE TO SET POSITION
    // GETTER ONLY
    cgp::vec2 position;

    bool is_idling;

    // display scale of the worm
    float scale;
    // body mesh - warning : do not include head
    // required by worm dust to place dust particles
    mesh body_mesh;
    hierarchy_mesh_drawable worm;

    // Initialize the worm
    Worm(float scale);

    // Just to avoid compilation warnings
    // Do nothing and should do nothing
    // else is called before some initialization stuff
    // in cgp and segfault yeepee
    Worm();

    // Return position (x, y, angle) after a given idle time
    vec3 idle(float idle_timer, Ground ground, float speed);

    // When having a target return (x, y, angle)
    vec3 worm_position_orientation(float v, vec2 cible);

    void set_position(float x, float y, Ground ground);
};
