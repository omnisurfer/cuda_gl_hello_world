#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cuda_gl_scene_utils.h"

int width = 1024;
int height = 768;

int current_height = 0;
int current_width = 0;

float near = 0.1f;	// clipping plane
float far = 100.0f; // clipping plane
float fovy = 67.0f; // field of view, degrees

float camera_speed = 5.0f; // 1 unit per second?
float camera_heading_speed = 100.0f; //30 degrees per second
float camera_heading = 0.0f; // y-rotation in degrees

float quaternion[4];

mat4* proj_matrix = NULL;
mat4 R;

vec4 forward(0.0, 0.0, -1.0f, 0.0f);
vec4 right(1.0, 0.0, 0.0f, 0.0f);
vec4 up(0.0, 1.0, 0.0f, 0.0f);
vec3 camera_position(0.0f, 0.0f, 5.0f);

/* 
Quaternion and versor operations taken from
OpenGL 4 Example Code.
Accompanies written series "Anton's OpenGL 4 Tutorials"
*/

/* create a unit quaternion q from an angle in degrees a, and an axis x,y,z */
void create_versor(float* q, float degrees, float x, float y, float z) {
	float radian = ONE_DEG_IN_RAD * degrees;
	q[0] = cosf(radian / 2.0f);	
	q[1] = sinf(radian / 2.0f) * x;
	q[2] = sinf(radian / 2.0f) * y;
	q[3] = sinf(radian / 2.0f) * z;
}

/* convert a unit quaternion q to a 4x4 matrix m */
void quat_to_mat4(float* m, const float* q) {
	float w = q[0];
	float x = q[1];
	float y = q[2];
	float z = q[3];
	m[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	m[1] = 2.0f * x * y + 2.0f * w * z;
	m[2] = 2.0f * x * z - 2.0f * w * y;
	m[3] = 0.0f;
	m[4] = 2.0f * x * y - 2.0f * w * z;
	m[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	m[6] = 2.0f * y * z + 2.0f * w * x;
	m[7] = 0.0f;
	m[8] = 2.0f * x * z + 2.0f * w * y;
	m[9] = 2.0f * y * z - 2.0f * w * x;
	m[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

/* normalise a quaternion in case it got a bit mangled */
void normalise_quat(float* q) {
	// norm(q) = q / magnitude (q)
	// magnitude (q) = sqrt (w*w + x*x...)
	// only compute sqrt if interior sum != 1.0
	float sum = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
	// NB: floats have min 6 digits of precision
	const float thresh = 0.0001f;
	if (fabs(1.0f - sum) < thresh) { return; }
	float mag = sqrt(sum);
	for (int i = 0; i < 4; i++) { q[i] = q[i] / mag; }
}

/* multiply quaternions to get another one. result=R*S */
void mult_quat_quat(float* result, const float* r, const float* s) {
	float w = s[0] * r[0] - s[1] * r[1] - s[2] * r[2] - s[3] * r[3];
	float x = s[0] * r[1] + s[1] * r[0] - s[2] * r[3] + s[3] * r[2];
	float y = s[0] * r[2] + s[1] * r[3] + s[2] * r[0] - s[3] * r[1];
	float z = s[0] * r[3] - s[1] * r[2] + s[2] * r[1] + s[3] * r[0];
	result[0] = w;
	result[1] = x;
	result[2] = y;
	result[3] = z;
	// re-normalise in case of mangling
	normalise_quat(result);
}

/*
END Quaternion and versor operators taken from
OpenGL 4 Example Code.
Accompanies written series "Anton's OpenGL 4 Tutorials"
*/

void init_camera() {
	create_versor(quaternion, -camera_heading, 0.0f, 1.0f, 0.0f);
	quat_to_mat4(R.m, quaternion);
}

bool configure_camera(
	float near_clipping_plane, 
	float far_clipping_plane, 
	float field_of_view_degrees,
	int viewport_width,
	int viewport_height,	
	mat4 *projection_matrix
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

mat4 move_camera(vec3 move, vec3 rotate) {
	
	// process roll
	if (true) {
		float q_roll[4];
		create_versor(q_roll, rotate.v[0], forward.v[0], forward.v[1], forward.v[2]);
		mult_quat_quat(quaternion, q_roll, quaternion);

		quat_to_mat4(R.m, quaternion);
		forward = R * vec4(0.0, 0.0, -1.0, 0.0);
		right = R * vec4(1.0, 0.0, 0.0, 0.0);
		up = R * vec4(0.0, 1.0, 0.0, 0.0);
	}

	// process pitch
	if (true) {
		float q_pitch[4];
		create_versor(q_pitch, rotate.v[1], right.v[0], right.v[1], right.v[2]);
		mult_quat_quat(quaternion, q_pitch, quaternion);

		quat_to_mat4(R.m, quaternion);
		forward = R * vec4(0.0, 0.0, -1.0, 0.0);
		right = R * vec4(1.0, 0.0, 0.0, 0.0);
		up = R * vec4(0.0, 1.0, 0.0, 0.0);
	}

	// process yaw
	if (true) {
		float q_yaw[4];
		create_versor(q_yaw, rotate.v[2], up.v[0], up.v[1], up.v[2]);
		mult_quat_quat(quaternion, q_yaw, quaternion);	
		quat_to_mat4(R.m, quaternion);

		forward = R * vec4(0.0, 0.0, -1.0, 0.0);
		right = R * vec4(1.0, 0.0, 0.0, 0.0);
		up = R * vec4(0.0, 1.0, 0.0, 0.0);
	}

	//printf("f x/y/z/w %f / %f / %f / %f\n", forward.v[0], forward.v[1], forward.v[2], forward.v[3]);
	//printf("r x/y/z/w %f / %f / %f / %f\n", right.v[0], right.v[1], right.v[2], right.v[3]);
	//printf("u x/y/z/w %f / %f / %f / %f\n", up.v[0], up.v[1], up.v[2], up.v[3]);

	// process with movement
	quat_to_mat4(R.m, quaternion);
	camera_position = camera_position + vec3( forward ) * -move.v[2]; // positive z is pointing towards viewer
	camera_position = camera_position + vec3( up ) * move.v[1];
	camera_position = camera_position + vec3( right ) * move.v[0];
	mat4 T = translate(identity_mat4(), vec3(camera_position));

	// checking for fp errors
	// printf ("dot fwd . up %f\n", dot (forward, up));
	// printf ("dot rgt . up %f\n", dot (right, up));
	// printf ("dot fwd . rgt %f\n", dot (forward, right));

	// printf("cam pos x/y/z %f / %f / %f\n", camera_position.v[0], camera_position.v[1], camera_position.v[2]);
	// printf("mov vec x/y/z %f / %f / %f\n", move.v[0], move.v[1], move.v[2]);

	mat4 view_matrix = inverse(R) * inverse(T);

	return view_matrix;
}

/* returns the view_matrix */
mat4 place_camera(vec3 camera_position, float camera_heading) {

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