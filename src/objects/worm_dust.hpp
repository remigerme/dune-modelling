#pragma once

#include "cgp/cgp.hpp"
#include "environment_dune.hpp"

struct WormDust {
    int n_particules;
    cgp::mesh_drawable particule;
    cgp::numarray<cgp::vec3> pos;
    cgp::numarray<cgp::vec4> phong;

    WormDust(int n_particules);

    // Just to avoid compilation warnings
    // Do nothing and should do nothing
    // else is called before some initialization stuff
    // in cgp and segfault yeepee
    WormDust(void);

    void display(environment_structure environment);
};
