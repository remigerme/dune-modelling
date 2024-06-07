#include "worm.hpp"
#include "../environment_dune.hpp"
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
            float phase = -Pi / 2;
            float y = shape * cos(i * frac + phase);
            float z = shape * sin(i * frac + phase);
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
                float coeff_perlin = 0.1 + 0.1 * kh / 10;
                m.position[idx].y = (1 + coeff_perlin * p_noise) * y;
                m.position[idx].z = (1 + coeff_perlin * p_noise) * z;
            } else {
                float coeff_perlin = 0.2;
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
    int concentique = 25;

    for (int kh = 0; kh < concentique; kh++) {
        std::cout << kh << std::endl;
        for (int ku = 0; ku < polygone; ku++) {
            float const v = ku / (polygone - 1.0f);
            m.uv.push_back({0, v});

            float rayon = sqrt(body.position[ku].y * body.position[ku].y +
                               body.position[ku].z * body.position[ku].z);
            float frac = 2 * Pi / polygone;
            float phase = -Pi / 2;
            float r = rayon * (concentique - kh) / concentique;
            float depth = mouth_shape(x_mouth, x_bottom, rayon, r);
            float y = r * cos(ku * frac + phase);
            float z = r * sin(ku * frac + phase);
            m.position.push_back(vec3{depth, y, z});
        }
    }

    // To close the mouth in the center
    float rayon = sqrt(body.position[0].y * body.position[0].y +
                       body.position[0].z * body.position[0].z);
    float depth = mouth_shape(x_mouth, x_bottom, rayon, 0);
    m.position.push_back(vec3{depth, 0, 0});
    m.uv.push_back({0, 0.5});

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
    body_mesh =
        create_worm_body(scale, 0.01 * scale, 20, x_mouth * scale, 0.4 * scale);
    worm_body.initialize_data_on_gpu(body_mesh);
    worm_body.material.phong.specular = 0;
    worm_body.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/worm_body.png", GL_REPEAT, GL_REPEAT);
    worm_body.material.color = {0.89f, 0.78f, 0.73f};

    mesh_drawable worm_head;
    worm_head.initialize_data_on_gpu(
        create_worm_head(body_mesh, x_mouth * scale, (x_mouth - 0.5) * scale));
    worm_head.material.phong.specular = 0;
    worm_head.texture.load_and_initialize_texture_2d_on_gpu(
        project::path + "assets/worm_head.png", GL_REPEAT, GL_REPEAT);
    worm_head.material.color = {0.89f, 0.78f, 0.67f};

    // Add the elements in the hierarchy
    worm.add(worm_body, "worm_body");
    worm.add(worm_head, "worm_head", "worm_body");
}

Worm::Worm() {
    // We're not supposed to use this one !
    assert(false);
}

void Worm::set_position(float x, float y, Ground ground) {
    float z = ground.get_height(x, y);
    worm["worm_body"].transform_local.translation = {x, y, z};
    position = {x, y};
}

vec3 Worm::idle(float idle_timer, Ground ground, float speed) {
    // Warning : hard-coded amplitude
    float t = idle_timer * speed;
    float xt = 65 * (cos(t) + 0.5 * cos(2 * t));
    float yt = 65 * (sin(t) + cos(t));

    // Derivatives
    float dxt = 65 * (-sin(t) - sin(2 * t));
    float dyt = 65 * (cos(t) - sin(t));
    // Calculate angle using atan2
    float angle = atan2(dyt, dxt);

    return vec3{xt, yt, angle};
}

vec3 Worm::worm_position_orientation(float v, vec2 cible) {
    // on rend dans l'ordre la nouvelle position selon x, la nouvelle position
    // selon y , et l'angle d'orientation du ver
    float dt = 0.1;
    float distance_mvt = dt * v;
    float x0 = cible[0] - position[0];
    float y0 = cible[1] - position[1];

    float d = sqrt(x0 * x0 + y0 * y0);
    float coef_homo = distance_mvt / d;
    float alpha = atan2(y0, x0);

    vec3 new_pos = {position[0] + coef_homo * x0, position[1] + coef_homo * y0,
                    alpha};
    return new_pos;
}
