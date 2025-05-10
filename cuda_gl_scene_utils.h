#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "maths_funcs.h"

mat4 create_camera(int viewport_width, int viewport_height);

//vec4 forwardd(0.0, 0.0, -1.0f, 0.0f);
//vec4 right(1.0, 0.0, 0.0f, 0.0f);
//vec4 up(0.0, 1.0, 0.0f, 0.0f);