#pragma once

#include "maths_funcs.h"

void configure_camera(
	float near_clipping_plane,
	float far_clipping_plane,
	float field_of_view_degrees,
	int viewport_width,
	int viewport_heigth,	
	mat4* projection_matrix
);

mat4 move_camera(vec3 move, vec3 rotate);

mat4 place_camera(vec3 cam_position, float cam_heading);