#include <cuda_gl_camera.h>

void BasicCUDAGLCamera::move_camera_x(float move_x) {
	move_.v[0] = move_x;
	camera_moved_ = true;
}
void BasicCUDAGLCamera::move_camera_y(float move_y) {
	move_.v[1] = move_y;
	camera_moved_ = true;
}
void BasicCUDAGLCamera::move_camera_z(float move_z) {
	move_.v[2] = move_z;
	camera_moved_ = true;
}

void BasicCUDAGLCamera::roll_camera(float roll) {
	rotate_.v[0] = roll;
	camera_moved_ = true;
}
void BasicCUDAGLCamera::pitch_camera(float pitch) {
	rotate_.v[1] = pitch;
	camera_moved_ = true;
}
void BasicCUDAGLCamera::yaw_camera(float yaw) {
	rotate_.v[2] = yaw;
	camera_moved_ = true;
}

void BasicCUDAGLCamera::init_camera() {
	create_versor(quaternion, -camera_heading, 0.0f, 1.0f, 0.0f);
	quat_to_mat4(R.m, quaternion);
}

bool BasicCUDAGLCamera::configure_camera(
	float near_clipping_plane,
	float far_clipping_plane,
	float field_of_view_degrees,
	int viewport_width,
	int viewport_height,
	mat4* projection_matrix
) {

	if (viewport_width == current_width && viewport_height == current_height) {
		return false;
	}
	else {
		current_width = viewport_width;
		current_height = viewport_height;

		printf("window width %i height %i\n", current_width, current_height);
	}

	near = near_clipping_plane;
	far = far_clipping_plane;
	fovy = field_of_view_degrees;

	height = viewport_height;
	width = viewport_width;

	proj_matrix = projection_matrix;

	float aspect = float((float)width / (float)height); // aspect ratio

	*proj_matrix = perspective(fovy, aspect, near, far);

	return true;
}

mat4 BasicCUDAGLCamera::move_camera() {

	// process roll
	if (true) {
		float q_roll[4];
		create_versor(q_roll, rotate_.v[0], forward.v[0], forward.v[1], forward.v[2]);
		mult_quat_quat(quaternion, q_roll, quaternion);

		quat_to_mat4(R.m, quaternion);
		forward = R * vec4(0.0, 0.0, -1.0, 0.0);
		right = R * vec4(1.0, 0.0, 0.0, 0.0);
		up = R * vec4(0.0, 1.0, 0.0, 0.0);
	}

	// process pitch
	if (true) {
		float q_pitch[4];
		create_versor(q_pitch, rotate_.v[1], right.v[0], right.v[1], right.v[2]);
		mult_quat_quat(quaternion, q_pitch, quaternion);

		quat_to_mat4(R.m, quaternion);
		forward = R * vec4(0.0, 0.0, -1.0, 0.0);
		right = R * vec4(1.0, 0.0, 0.0, 0.0);
		up = R * vec4(0.0, 1.0, 0.0, 0.0);
	}

	// process yaw
	if (true) {
		float q_yaw[4];
		create_versor(q_yaw, rotate_.v[2], up.v[0], up.v[1], up.v[2]);
		mult_quat_quat(quaternion, q_yaw, quaternion);
		quat_to_mat4(R.m, quaternion);

		forward = R * vec4(0.0, 0.0, -1.0, 0.0);
		right = R * vec4(1.0, 0.0, 0.0, 0.0);
		up = R * vec4(0.0, 1.0, 0.0, 0.0);
	}

	// process with movement
	quat_to_mat4(R.m, quaternion);
	camera_position = camera_position + vec3(forward) * -move_.v[2]; // positive z is pointing towards viewer
	camera_position = camera_position + vec3(up) * move_.v[1];
	camera_position = camera_position + vec3(right) * move_.v[0];
	mat4 T = translate(identity_mat4(), vec3(camera_position));

	mat4 view_matrix = inverse(R) * inverse(T);

	move_.v[0] = 0.0f;
	move_.v[1] = 0.0f;
	move_.v[2] = 0.0f;

	rotate_.v[0] = 0.0f;
	rotate_.v[1] = 0.0f;
	rotate_.v[2] = 0.0f;

	return view_matrix;

}

mat4 BasicCUDAGLCamera::move_camera(vec3 move, vec3 rotate) {

	move_ = move;
	rotate_ = rotate;

	mat4 view_matrix = move_camera();

	return view_matrix;
}

/* returns the view_matrix */
mat4 BasicCUDAGLCamera::place_camera(vec3 camera_position, float camera_heading) {

	mat4 T = translate(identity_mat4(), vec3(-camera_position.v[0], -camera_position.v[1], -camera_position.v[2]));

	// make a quaternion representing negated initial camera orientation
	float quaternion[4];
	create_versor(quaternion, -camera_heading, 0.0f, 1.0f, 0.0f);
	// convert the quaternion to a rotation matrix (just an array of 16 floats)
	quat_to_mat4(R.m, quaternion);

	mat4 view_matrix;
	view_matrix = R * T;

	return view_matrix;
}

