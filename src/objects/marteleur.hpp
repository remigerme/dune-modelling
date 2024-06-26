#pragma once

#include "../terrain/ground.hpp"
#include "cgp/cgp.hpp"

using cgp::hierarchy_mesh_drawable;
using cgp::vec3;

struct Marteleur {
    hierarchy_mesh_drawable marteleur;

    // Initialize the mesh of the marteleur and the mesh_drawable
    // Useless argument but used to differ this ctor from the next one
    // See below the reason
    Marteleur(bool _unused);

    // Just to avoid compilation warnings
    // Do nothing and should do nothing
    // else is called before some initialization stuff
    // in cgp and segfault yeepee
    Marteleur();

    // Sets position of whole hierarchy at scaled coordinates (x, y)
    // Computes z based on ground height
    void set_position(float xy[2], Ground ground);
};
