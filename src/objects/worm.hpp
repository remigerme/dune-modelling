#include "cgp/cgp.hpp"

using cgp::hierarchy_mesh_drawable;
using cgp::mesh;

struct Worm {
    // body mesh - warning : do not include head
    // required by worm dust to place dust particles
    mesh body;

    hierarchy_mesh_drawable worm;

    // Initialize the mesh of the marteleur and the mesh_drawable
    // Useless argument but used to differ this ctor from the next one
    // See below the reason
    Worm(bool _unused);

    // Just to avoid compilation warnings
    // Do nothing and should do nothing
    // else is called before some initialization stuff
    // in cgp and segfault yeepee
    Worm();
};