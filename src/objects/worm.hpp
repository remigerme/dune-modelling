#pragma once

#include "../terrain/ground.hpp"
#include "cgp/cgp.hpp"

using cgp::hierarchy_mesh_drawable;
using cgp::mesh;

struct Worm {
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

    void set_position(float x, float y, Ground ground);
};
