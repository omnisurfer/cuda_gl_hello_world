/*
* Using nvdia particles CUDA example for guidance.
*/
#define _USE_MATH_DEFINES
#include <cmath>

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

// Utils
#include "stb_image.h"

// Anton's OpenGL Tutorial Includes
#include "maths_funcs.h" 
#include "obj_parser.h"

#include "cuda_gl_setup_utils.h"
#include "cuda_gl_scene_utils.h"

// drawings
#include "simple_triangle.h"
#include "q_camera_triangle.h"
#include "cube_map.h"

// CUDA externs
extern "C" int execute_kernel();

/* Create a shader https://antongerdelan.net/opengl/compute.html */
// TBD

int main(int arc, char** argvv) {

	printf("Starting cuda_gl_hello_world\n");

	GLFWwindow* window;

	// TODO turn this into a function?
	window = init_gl(1024, 1024);

	if (!window)
	{
		printf("Failed to create Open GL window");
		return -1;
	}

	draw_q_camera_triangle(window);

	if (false) {
		execute_kernel();

		window = init_gl(1024, 1024);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}

		// draw_cube_map(window);
		draw_simple_triangle(window);
	}

	return 0;
}