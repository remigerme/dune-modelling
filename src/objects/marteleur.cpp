#include "marteleur.hpp"
#include <cmath>

using namespace cgp;

mesh mesh_primitive_conetronqued(float radius_bas, float z_bas,
                                 float radius_haut, float z_haut) {
    mesh m;
    int polygone = 25;

    for (int i = 0; i < polygone; i++) {
        float frac = 2 * Pi / polygone;
        float x_bas = radius_bas * cos(i * frac);
        float y_bas = radius_bas * sin(i * frac);

        m.position.push_back(vec3{x_bas, y_bas, z_bas});

        float x_haut = radius_haut * cos(i * frac);
        float y_haut = radius_haut * sin(i * frac);

        m.position.push_back(vec3{x_haut, y_haut, z_haut});
    }

    for (int ku = 0; ku < 2 * polygone - 2; ku += 2) {
        unsigned int idx = ku;
        uint3 triangle_1 = {idx + 2, idx + 1, idx};
        uint3 triangle_2 = {idx + 2, idx + 3, idx + 1};
        m.connectivity.push_back(triangle_1);
        m.connectivity.push_back(triangle_2);
    }
    uint3 triangle_1 = {0, 2 * polygone - 1, 2 * polygone - 2};
    uint3 triangle_2 = {0, 1, 2 * polygone - 1};
    m.connectivity.push_back(triangle_1);
    m.connectivity.push_back(triangle_2);

    m.fill_empty_field();
    return m;
};

Marteleur::Marteleur(bool _unused) {
    // Create the hierarchy
    // ************************************ //

    // Initialize the temporary mesh_drawable that will be inserted in the
    // hierarchy
    mesh_drawable tube_central;
    mesh_drawable corrole;
    mesh_drawable disc1;
    mesh_drawable disc2;
    mesh_drawable bas;
    mesh_drawable haut;
    mesh_drawable tete;
    mesh_drawable cone_bas;
    mesh_drawable pas_de_vis;
    mesh_drawable disc3;
    mesh_drawable pas_bas;

    // Create the geometry of the meshes

    tube_central.initialize_data_on_gpu(
        mesh_primitive_cylinder(0.05f, {0, 0, 0}, {0, 0, 1.5}));
    corrole.initialize_data_on_gpu(
        mesh_primitive_cylinder(0.3f, {0, 0, 0}, {0, 0, 0.05}));
    disc1.initialize_data_on_gpu(mesh_primitive_disc(0.3f));
    disc2.initialize_data_on_gpu(mesh_primitive_disc(0.3f));
    bas.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.21, 0, 0.25, 0.25));
    haut.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.25, 0, 0.21, 0.55));
    tete.initialize_data_on_gpu(
        mesh_primitive_ellipsoid({0.21, 0.21, 0.21}, {0, 0, 0}));
    cone_bas.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.15, 0, 0.21, 0.1));
    pas_de_vis.initialize_data_on_gpu(
        mesh_primitive_cylinder(0.15, {0, 0, 0}, {0, 0, 0.15}));
    disc3.initialize_data_on_gpu(mesh_primitive_disc(0.25f));
    pas_bas.initialize_data_on_gpu(
        mesh_primitive_conetronqued(0.05, 0, 0.15, 0.05));

    // Set the color of some elements
    haut.material.color = {1.0f, 0.5f, 0.1f};

    // Add the elements in the hierarchy

    marteleur.add(tube_central, "tube_central");
    marteleur.add(corrole, "corrole", "tube_central", {0, 0, 1});
    marteleur.add(disc1, "disc1", "corrole", {0, 0, 0.05});
    marteleur.add(disc2, "disc2", "corrole", {0, 0, 0});
    marteleur.add(bas, "bas", "tube_central", {0, 0, 0.75});
    marteleur.add(haut, "haut", "tube_central", {0, 0, 1});
    marteleur.add(tete, "tete", "haut", {0, 0, 0.55});
    marteleur.add(disc3, "disc3", "haut", {0, 0, 0});
    marteleur.add(cone_bas, "cone_bas", "tube_central", {0, 0, 0.65});
    marteleur.add(pas_de_vis, "pas_de_vis", "tube_central", {0, 0, 0.50});
    marteleur.add(pas_bas, "pas_bas", "tube_central", {0, 0, 0.45});
}

Marteleur::Marteleur() {
    // We're not supposed to use this one !
    assert(false);
}
