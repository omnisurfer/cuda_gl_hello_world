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

#include <cuda_gl_common.h>

// scenes from the book
#include "simple_triangle.h"

#include "template_scene.h"
#include "quat_camera_spheres.h"
#include "cube_map.h"

#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080

// CUDA externs
extern "C" int execute_kernel();

/* Create a shader https://antongerdelan.net/opengl/compute.html */
// TBD

int main(int arc, char** argvv) {
	GLFWwindow* window = NULL;

	CUDAGLCommon cuda_gl_common;

	printf("Starting cuda_gl_hello_world\n");

	if (true) {
		execute_kernel();
	}

	if (true) {
		window = cuda_gl_common.init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}

		draw_template_scene(window, &cuda_gl_common);
	}
	
	if (false) {
		window = cuda_gl_common.init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}

		draw_quat_cam_spheres(window, &cuda_gl_common);
	}
	
	if (false) {

		window = cuda_gl_common.init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}		

		draw_simple_triangle(window, &cuda_gl_common);
	}

	if (false) {

		window = cuda_gl_common.init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}
		draw_cube_map(window, &cuda_gl_common);
	}

	return 0;
}