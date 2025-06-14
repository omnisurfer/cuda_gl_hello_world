/*
* Using nvdia particles CUDA example for guidance.
*/
// #define _USE_MATH_DEFINES
// #include <cmath>

// OpenGL Graphics includes
/* https://www.glfw.org/documentation.html */
/* https://www.youtube.com/watch?v=uO__ntYT-2Q */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// CUDA
#include <cuda_runtime.h>

// stdlib
#include <cstdio>
#include <fstream>
#include <iterator>
#include <vector>
#include <assert.h>

// scenes from the book
#include "simple_triangle.h"
#include "quat_camera_spheres.h"
#include "cube_map.h"

#define DISPLAY_HEIGHT 1080
#define DISPLAY_WIDTH 1920

// CUDA externs
extern "C" int execute_kernel();

/* Create a shader https://antongerdelan.net/opengl/compute.html */
// TBD

int main(int arc, char** argvv) {
	GLFWwindow* window;

	printf("Starting cuda_gl_hello_world\n");

	if (true) {
		execute_kernel();
	}
	
	if (true) {
		window = init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}

		draw_quat_cam_spheres(window);
	}

	if (true) {

		window = init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}		
		draw_simple_triangle(window);
	}

	if (false) {

		window = init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}
		draw_cube_map(window);
	}

	return 0;
}