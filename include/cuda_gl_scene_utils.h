#pragma once

#include "maths_funcs.h"

void configure_camera(
	float near_clipping_plane,
	float far_clipping_plane,
	float field_of_view_degrees,
	float viewport_heigth,
	float viewport_width,
	mat4* projection_matrix
);

mat4 move_camera(vec3 move);

mat4 update_camera(vec3 cam_position, float cam_heading);