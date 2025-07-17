/*
* Using nvdia particles CUDA example for guidance.
*/

// CUDA
#include <cuda_runtime.h>

#include <cuda_gl_common.h>

// scenes from the book
#include "template_scene.h"
#include "simple_triangle.h"
#include "quat_camera_spheres.h"
#include "phong_spheres.h"
#include "texture_load.h"

#include "cube_map.h"

#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080

// CUDA externs
extern "C" {
	int execute_kernel();
	int execute_image_rotation_kernel(
		const float* input_image_data,
		float* output_image_data,
		int x_dimension,
		int y_dimension,
		int stride,
		float angle_degrees
	);
}

/* Create a shader https://antongerdelan.net/opengl/compute.html */

int main(int arc, char** argvv) {
	GLFWwindow* window = NULL;

	CUDAGLCommon cuda_gl_common;

	printf("Starting cuda_gl_hello_world\n");
		
	if (true) {
		execute_kernel();
	}

	if (true) {
		
		const int x_dimension = 64;	
		const int y_dimension = 64;
		const int stride = 4;		// 4 bytes per channel, RGBA		
		const float rotation_angle_degrees = 90.0;
		const int image_data_bytes_size = sizeof(float) * x_dimension * y_dimension;

		// may need to change to float* to represent 4 bytes of RGBA
		float input_image_data[x_dimension * y_dimension];
		float output_image_data[x_dimension * y_dimension];		

		// fill with garbage
		for (int i = 0; i < x_dimension * y_dimension; i++) {
			input_image_data[i] = 65 + i;
			output_image_data[i] = 0;
		}

		int success = execute_image_rotation_kernel(
			input_image_data,
			output_image_data,
			x_dimension,
			y_dimension,
			stride,
			rotation_angle_degrees
		);

		if (success > 0) {
			fprintf(stderr, "Image rotation kernel failed to execute.\n");
		}

		// print results for debugging		
		for (int i = 0; i < x_dimension * y_dimension; i++) {

			if (i > 256)
				break;

			printf("%i, %f, %f\n", i, input_image_data[i], output_image_data[i]);
		}

		// printf("Result 0x%02X\n", output_image_data[0]);
	}

	if (false) {
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

		draw_simple_triangle(window, &cuda_gl_common);
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

		draw_phong_spheres(window, &cuda_gl_common);
	}

	if (false) {
		
		window = cuda_gl_common.init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}		

		draw_texture_load(window, &cuda_gl_common);		
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