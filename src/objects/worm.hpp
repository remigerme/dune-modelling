#include "cgp/cgp.hpp"

float worm_shape(float a, float r, float x);
float mouth_shape(float x_mouth, float x_bottom, float R, float r);

cgp::mesh create_worm_body(float radius, float h, int length_h, float x_mouth,
                           float a);

cgp::mesh create_worm_head(cgp::mesh body, float x_mouth, float x_mottom);