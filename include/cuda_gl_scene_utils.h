#pragma once

#include "maths_funcs.h"

#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0f;

mat4 create_camera(int viewport_width, int viewport_height);

vec4 forwardd(0.0, 0.0, -1.0f, 0.0f);
vec4 right(1.0, 0.0, 0.0f, 0.0f);
vec4 up(0.0, 1.0, 0.0f, 0.0f);