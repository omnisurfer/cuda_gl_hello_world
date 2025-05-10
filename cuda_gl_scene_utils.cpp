#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cuda_gl_scene_utils.h"

/* camera from 21_cube_mapping */
mat4 create_camera(int viewport_width, int viewport_height) {

	float near = 0.1f;	// clipping plane
	float far = 100.0f; // clipping plane
	float fovy = 67.0f; // field of view, degrees
	float aspect = float(viewport_width / viewport_height); // aspect ratio

	mat4 projection_matrix;
	vec3 camera_position(0.0f, 0.0f, 5.0f);

	projection_matrix = perspective(fovy, aspect, near, far);

	float cmaera_speed = 5.0f; // 1 unit per second?
	float camera_heading_speed = 100.0f; //30 degrees per second
	float camera_heading = 0.0f; // y-rotation in degrees

	mat4 T = translate(identity_mat4(), vec3(-camera_position.v[0], -camera_position.v[1], -camera_position.v[2]));
	mat4 R = rotate_y_deg(identity_mat4(), -camera_heading);
	versor q = quat_from_axis_deg(-camera_heading, 0.0f, 1.0f, 0.0f);

	mat4 view_matrix;
	view_matrix = R * T;

	return view_matrix;
}