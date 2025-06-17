#pragma once

#include <cuda_gl_include.h>

class CUDAGLCamera {

public:

	double delta_time = 0.0f;

	mat4 view_matrix;
	mat4 projection_matrix;

	int view_matrix_location = 0;
	int project_matrix_location = 0;

	vec3 camera_position;

	bool camera_moved_ = false;

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

	float quaternion[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	mat4* proj_matrix = NULL;
	mat4 R;

	vec3 move_;
	vec3 rotate_;

	vec4 forward;
	vec4 right;
	vec4 up;

	double now = 0.0f;
	const double fps_limit_frequency = 60.0;
	const double fps_limit_period = 1 / fps_limit_frequency;
	double last_update_time = 0;
	double last_frame_time = 0;


public:
	CUDAGLCamera() {
		printf("starting cmaera\n");
	}
	
	void init_camera();

	bool update_frame(GLFWwindow *window) {

		bool update_frame = false;
		double fps = 0.0f;
		char temp[256];

		now = glfwGetTime();
		delta_time = now - last_update_time;

		if ((now - last_frame_time) >= fps_limit_period) {

			fps = 1 / (now - last_frame_time);			
			sprintf_s(temp, "FPS %.2lf, delta %f", fps, delta_time);
			glfwSetWindowTitle(window, temp);

			update_frame = true;

			last_frame_time = now;
		}
		
		last_update_time = now;
		
		return update_frame;
	}

	bool configure_camera(int viewport_width, int viewport_height);

	bool configure_camera(
		float near_clipping_plane,
		float far_clipping_plane,
		float field_of_view_degrees,
		int viewport_width,
		int viewport_heigth
	);

	void move_camera_x(float move_x);
	void move_camera_y(float move_y);
	void move_camera_z(float move_z);

	void roll_camera(float roll);
	void pitch_camera(float pitch);
	void yaw_camera(float yaw);

	mat4 move_camera();
	mat4 move_camera(vec3 move, vec3 rotate);

	mat4 place_camera(vec3 position);

private:
	/*
	Quaternion and versor operations taken from
	OpenGL 4 Example Code.
	Accompanies written series "Anton's OpenGL 4 Tutorials"
	*/

	/* create a unit quaternion q from an angle in degrees a, and an axis x,y,z */
	void create_versor(float* q, float degrees, float x, float y, float z) {
		float radian = float(ONE_DEG_IN_RAD * degrees);
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
};