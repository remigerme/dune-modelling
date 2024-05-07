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