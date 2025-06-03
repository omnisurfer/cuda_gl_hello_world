#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <maths_funcs.h>

void init_camera();

bool configure_camera(
	float near_clipping_plane,
	float far_clipping_plane,
	float field_of_view_degrees,
	int viewport_width,
	int viewport_heigth,	
	mat4* projection_matrix
);

void move_camera_x(float move_x);
void move_camera_y(float move_y);
void move_camera_z(float move_z);

void roll_camera(float roll);
void pitch_camera(float pitch);
void yaw_camera(float yaw);

mat4 move_camera();
mat4 move_camera(vec3 move, vec3 rotate);

mat4 place_camera(vec3 cam_position, float cam_heading);