#include "worm.hpp"
#include "../noise/perlin.hpp"

using namespace cgp;

// Fonction f(x) = r*exp(-ax^2), pour faire la forme du ver
//         _____
//        /     \
//       /
//______/
//
float worm_shape(float a, float r, float x) {
    float f_exp = r * exp(-a * x * x);
    float lim = 0.75 * r;
    if (lim < f_exp) {
        return (f_exp);
    } else {
        return (lim);
    }
}

float mouth_shape(float x_mouth, float x_bottom, float R, float r) {
    float mouth_depth = x_mouth - x_bottom;
    float a = mouth_depth / (R * R);
    return (a * r * r + x_bottom);
}

cgp::mesh create_worm_body(float radius, float h, int length_h, float x_mouth,
                           float a) {
    // h le pas, et lenght_h le nombre de pas qui est donc proportionnel à la
    // longeur

    mesh m;
    int polygone = 30;

    for (int k = 0; k < length_h; k++) {
        float abscicsse = x_mouth - k * h * length_h;
        float shape = worm_shape(a, radius, abscicsse);
        for (int i = 0; i < polygone; i++) {
            float const u = k / (length_h - 1.0f);
            float const v = i / (polygone - 1.0f);
            m.uv.push_back({u, v});

            float frac = 2 * Pi / polygone;
            float y = shape * cos(i * frac);
            float z = shape * sin(i * frac);

            m.position.push_back(vec3{abscicsse, y, z});
        }
    }

    perlin_noise_parameters p = {0.3f, 2.0f, 3, 1};
    for (int kh = 0; kh < length_h; kh++) {
        for (int ku = 0; ku < polygone; ku++) {
            int idx = kh * polygone + ku;
            float z = m.position[idx].z;
            float y = m.position[idx].y;
            float p_noise = noise_perlin(m.position[idx], p.octave,
                                         p.persistency, p.frequency);

            if (kh < 10) {
                float coeff_perlin = 0.2 + 0.1 * kh / 10;
                m.position[idx].y = (1 + coeff_perlin * p_noise) * y;
                m.position[idx].z = (1 + coeff_perlin * p_noise) * z;
            } else {
                float coeff_perlin = 0.3;
                m.position[idx].y = (1 + coeff_perlin * p_noise) * y;
                m.position[idx].z = (1 + coeff_perlin * p_noise) * z;
            }
        }
    }

    for (int kh = 0; kh < length_h - 1; kh++) {
        for (int ku = 0; ku < polygone - 1; ku++) {

            uint3 triangle_1 = {polygone * kh + ku + 1, polygone * kh + ku,
                                polygone * (kh + 1) + ku};
            uint3 triangle_2 = {polygone * kh + ku + 1,
                                polygone * (kh + 1) + ku,
                                polygone * (kh + 1) + ku + 1};
            m.connectivity.push_back(triangle_1);
            m.connectivity.push_back(triangle_2);
        }
        uint3 triangle_1 = {polygone * kh, polygone * (kh + 1) - 1,
                            polygone * (kh + 1)};
        uint3 triangle_2 = {polygone * (kh + 1), polygone * (kh + 1) - 1,
                            polygone * (kh + 2) - 1};
        m.connectivity.push_back(triangle_1);
        m.connectivity.push_back(triangle_2);
    }

    m.fill_empty_field();
    return m;
}

cgp::mesh create_worm_head(cgp::mesh body, float x_mouth, float x_bottom) {

    mesh m;
    int polygone = 30;
    int concentique = 15;

    for (int kh = 0; kh < concentique; kh++) {
        for (int ku = 0; ku < polygone; ku++) {
            float const u = kh / (concentique - 1.0f);
            float const v = ku / (polygone - 1.0f);
            m.uv.push_back({1, v});

            float rayon = sqrt(body.position[ku].y * body.position[ku].y +
                               body.position[ku].z * body.position[ku].z);
            float frac = 2 * Pi / polygone;
            float r = rayon * (concentique - kh) / concentique;
            float depth = mouth_shape(x_mouth, x_bottom, rayon, r);
            float y = r * cos(ku * frac);
            float z = r * sin(ku * frac);
            m.position.push_back(vec3{depth, y, z});
        }
    }

    // To close the mouth in the center
    float rayon = sqrt(body.position[0].y * body.position[0].y +
                       body.position[0].z * body.position[0].z);
    float depth = mouth_shape(x_mouth, x_bottom, rayon, 0);
    m.position.push_back(vec3{depth, 0, 0});

    for (int kh = 0; kh < concentique - 1; kh++) {
        for (int ku = 0; ku < polygone - 1; ku++) {

            uint3 triangle_1 = {polygone * kh + ku + 1,
                                polygone * (kh + 1) + ku, polygone * kh + ku};
            uint3 triangle_2 = {polygone * kh + ku + 1,
                                polygone * (kh + 1) + ku + 1,
                                polygone * (kh + 1) + ku};
            m.connectivity.push_back(triangle_1);
            m.connectivity.push_back(triangle_2);
        }
        uint3 triangle_1 = {polygone * kh, polygone * (kh + 1),
                            polygone * (kh + 1) - 1};
        uint3 triangle_2 = {polygone * (kh + 1), polygone * (kh + 2) - 1,
                            polygone * (kh + 1) - 1};
        m.connectivity.push_back(triangle_1);
        m.connectivity.push_back(triangle_2);
    }

    // To close the mouth in the center
    for (int ku = 0; ku < polygone; ku++) {
        uint3 triangle = {polygone * (concentique - 1) + (ku + 1) % polygone,
                          polygone * concentique,
                          polygone * (concentique - 1) + ku};
        m.connectivity.push_back(triangle);
    }

    m.fill_empty_field();
    return m;
}

Worm::Worm(float scale) {
    this->scale = scale;
    // Create the hierarchy

    // Create the geometry of the meshes
    // And mesh drawable which will be inserted in the hierarchy
    float x_mouth = 0.4;
    mesh_drawable worm_body;
    body_mesh = create_worm_body(1.0, 0.01, 20, x_mouth, 0.4);
    worm_body.initialize_data_on_gpu(body_mesh);
    worm_body.material.phong.specular = 0;
    worm_body.texture.load_and_initialize_texture_2d_on_gpu(
        "/home/isaline.jouve/dune-modelling/assets/cliff.jpg", GL_REPEAT,
        GL_REPEAT);

    mesh_drawable worm_head;
    worm_head.initialize_data_on_gpu(
        create_worm_head(body_mesh, x_mouth, x_mouth - 0.5));

    // Add the elements in the hierarchy
    worm.add(worm_body, "worm_body");
    worm.add(worm_head, "worm_head", "worm_body");

    // Display scaling
    for (auto &e : worm.elements)
        e.drawable.model.set_scaling(scale);
}

Worm::Worm() {
    // We're not supposed to use this one !
    assert(false);
}
