#pragma once

#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <maths_funcs.h>

#include <cuda_gl_camera.h>

class CUDAGLUserInput {

public:

	static bool move_forward;
	static bool move_backward;

	static bool move_left;
	static bool move_right;

	static bool move_up;
	static bool move_down;

	static bool roll_right;
	static bool roll_left;

	static bool pitch_up;
	static bool pitch_down;

	static bool yaw_right;
	static bool yaw_left;

	static bool mouse_button_left;
	static bool mouse_button_middle;
	static bool mouse_button_right;

	static bool reload_shader_key_pressed;

	void init_cuda_gl_user_input(GLFWwindow* window) {

		glfwSetKeyCallback(window, gl_key_callback);
		glfwSetMouseButtonCallback(window, gl_mouse_button_callback);
	}

	static void gl_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		// if (scene_key_callback_function != nullptr) {
		//	scene_key_callback_function(window, key, scancode, action, mods);
		// }
		scene_key_callback_function(window, key, scancode, action, mods);
	}

	static void gl_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

		// if (scene_mouse_button_callback_function != nullptr) {
		// 	scene_mouse_button_callback_function(window, button, action, mods);
		// }
		scene_mouse_button_callback_function(window, button, action, mods);
	}

	static void scene_key_callback_function(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void scene_mouse_button_callback_function(GLFWwindow* window, int button, int action, int mods);

	void process_movements(CUDAGLCamera* camera);

	vec3 process_mouse(GLFWwindow* window, CUDAGLCamera* camera);

	vec3 get_ray_from_mouse_coords(GLFWwindow* window, CUDAGLCamera* camera, float mouse_x, float mouse_y);
};