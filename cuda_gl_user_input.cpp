#include <cuda_gl_user_input.h>

bool CUDAGLUserInput::move_forward = false;
bool CUDAGLUserInput::move_backward = false;

bool CUDAGLUserInput::move_left = false;
bool CUDAGLUserInput::move_right = false;

bool CUDAGLUserInput::move_up = false;
bool CUDAGLUserInput::move_down = false;

bool CUDAGLUserInput::roll_right = false;
bool CUDAGLUserInput::roll_left = false;

bool CUDAGLUserInput::pitch_up = false;
bool CUDAGLUserInput::pitch_down = false;

bool CUDAGLUserInput::yaw_right = false;
bool CUDAGLUserInput::yaw_left = false;

bool CUDAGLUserInput::mouse_button_left = false;
bool CUDAGLUserInput::mouse_button_middle = false;
bool CUDAGLUserInput::mouse_button_right = false;

bool CUDAGLUserInput::reload_shader_key_pressed = false;

vec3 CUDAGLUserInput::get_ray_from_mouse_coords(GLFWwindow* window, CUDAGLCamera* camera, float mouse_x, float mouse_y) {

	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	// screen space (viewport coordinates)
	float x = (2.0f * mouse_x) / (float)window_width - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / (float)window_height;
	float z = 1.0f;

	vec3 ray_normalized_device_space = vec3(x, y, z);

	if (false) {
		printf("dev space %f %f %f\n",
			ray_normalized_device_space.v[0],
			ray_normalized_device_space.v[1],
			ray_normalized_device_space.v[2]
		);
	}

	// clip space
	vec4 ray_clip_space = vec4(ray_normalized_device_space.v[0], ray_normalized_device_space.v[1], -1.0, 1.0);

	if (false) {
		printf("clip space %f %f %f %f\n",
			ray_clip_space.v[0],
			ray_clip_space.v[1],
			ray_clip_space.v[2],
			ray_clip_space.v[3]
		);
	}

	// eye space
	vec4 ray_eye_space = inverse(camera->projection_matrix) * ray_clip_space;

	if (false) {
		printf("eye space pre %f %f %f %f\n",
			ray_eye_space.v[0],
			ray_eye_space.v[1],
			ray_eye_space.v[2],
			ray_eye_space.v[3]
		);
	}

	ray_eye_space = vec4(ray_eye_space.v[0], ray_eye_space.v[1], -1.0, 0.0);

	if (false) {
		printf("eye space post %f %f %f %f\n",
			ray_eye_space.v[0],
			ray_eye_space.v[1],
			ray_eye_space.v[2],
			ray_eye_space.v[3]
		);
	}

	// world space
	vec3 ray_world_space = vec3(inverse(camera->view_matrix) * ray_eye_space);

	if (false) {
		printf("world space raw %f %f %f\n",
			ray_world_space.v[0],
			ray_world_space.v[1],
			ray_world_space.v[2]
		);
	}

	ray_world_space = normalise(ray_world_space);

	if (false) {
		printf("world space norm %f %f %f\n",
			ray_world_space.v[0],
			ray_world_space.v[1],
			ray_world_space.v[2]
		);
	}

	return ray_world_space;
}

void CUDAGLUserInput::scene_key_callback_function(GLFWwindow* window, int key, int scancode, int action, int mods) {

	printf("scene_key_callback_function %i %i %i %i\n", key, scancode, action, mods);

	// forward, back, left, right, up, down
	if (glfwGetKey(window, GLFW_KEY_W)) {
		move_forward = true;
	}
	else {
		move_forward = false;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		move_left = true;
	}
	else {
		move_left = false;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		move_backward = true;
	}
	else {
		move_backward = false;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		move_right = true;
	}
	else {
		move_right = false;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		move_up = true;
	}
	else {
		move_up = false;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		move_down = true;
	}
	else {
		move_down = false;
	}

	// roll, pitch, yaw
	if (glfwGetKey(window, GLFW_KEY_Z)) {
		roll_left = true;
	}
	else {
		roll_left = false;
	}
	if (glfwGetKey(window, GLFW_KEY_X)) {
		roll_right = true;
	}
	else {
		roll_right = false;
	}
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		pitch_up = true;
	}
	else {
		pitch_up = false;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		pitch_down = true;
	}
	else {
		pitch_down = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		yaw_left = true;
	}
	else {
		yaw_left = false;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		yaw_right = true;
	}
	else {
		yaw_right = false;
	}

	// reload shader
	if (key == GLFW_KEY_R && action == 0) {
		reload_shader_key_pressed = true;		
	}
	else {
		reload_shader_key_pressed = false;
	}
}

void CUDAGLUserInput::scene_mouse_button_callback_function(GLFWwindow* window, int button, int action, int mods) {

	if (action == GLFW_PRESS) {

		printf("press ");

		if (button == GLFW_MOUSE_BUTTON_1) {
			printf("button 1\n");
			mouse_button_left = true;
		}

		if (button == GLFW_MOUSE_BUTTON_2) {
			printf("button 2\n");
			mouse_button_right = true;
		}

		if (button == GLFW_MOUSE_BUTTON_3) {
			printf("button 3\n");
			mouse_button_middle = true;
		}
	}
	else if (action == GLFW_RELEASE) {

		printf("relase ");

		if (button == GLFW_MOUSE_BUTTON_1) {
			printf("button 1\n");
			mouse_button_left = false;
		}

		if (button == GLFW_MOUSE_BUTTON_2) {
			printf("button 2\n");
			mouse_button_right = false;
		}

		if (button == GLFW_MOUSE_BUTTON_3) {
			printf("button 3\n");
			mouse_button_middle = false;
		}
	}
}

void CUDAGLUserInput::process_movements(CUDAGLCamera* camera) {
	
	float move_delta = camera->camera_speed * camera->delta_time;

	bool camera_moved = false;

	if (true) {
		if (move_left) {
			camera_moved = true;
			camera->move_camera_x(-move_delta);
		}
		if (move_right) {
			camera_moved = true;
			camera->move_camera_x(move_delta);
		}
		if (move_up) {
			camera_moved = true;
			camera->move_camera_y(-move_delta);
		}
		if (move_down) {
			camera_moved = true;
			camera->move_camera_y(move_delta);
		}
		if (move_forward) {
			camera_moved = true;
			camera->move_camera_z(-move_delta);
		}
		if (move_backward) {
			camera_moved = true;
			camera->move_camera_z(move_delta);
		}

		// ROTATIONS		
		float rotation_delta = camera->camera_heading_speed * (float)camera->delta_time;
		if (roll_left) {
			camera_moved = true;
			camera->roll_camera(rotation_delta);
		}
		if (roll_right) {
			camera_moved = true;
			camera->roll_camera(-rotation_delta);
		}
		if (pitch_up) {
			camera_moved = true;
			camera->pitch_camera(rotation_delta);
		}
		if (pitch_down) {
			camera_moved = true;
			camera->pitch_camera(-rotation_delta);
		}
		if (yaw_left) {
			camera_moved = true;
			camera->yaw_camera(rotation_delta);
		}
		if (yaw_right) {
			camera_moved = true;
			camera->yaw_camera(-rotation_delta);
		}
	}

	if (camera_moved) {
		camera->move_camera();
		glUniformMatrix4fv(camera->view_matrix_location, 1, GL_FALSE, camera->view_matrix.m);
	}
}

vec3 CUDAGLUserInput::process_mouse(GLFWwindow* window, CUDAGLCamera* camera) {

	double x_position, y_position;

	glfwGetCursorPos(window, &x_position, &y_position);

	// printf("x_pos %f y_pos %f\n", x_position, y_position);

	return get_ray_from_mouse_coords(window, camera, (float)x_position, (float)y_position);
}