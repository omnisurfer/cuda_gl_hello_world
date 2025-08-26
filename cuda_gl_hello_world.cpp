/*
* Using nvdia particles CUDA example for guidance.
*/

// CUDA
#include <cuda_runtime.h>

#include <cuda_gl_common.h>

#include <string>

// scenes from the book
#include "template_scene.h"
#include "simple_triangle.h"
#include "quat_camera_spheres.h"
#include "phong_spheres.h"
#include "texture_load.h"

#include "restructure.h"

#include "cube_map.h"

// Have to define here since it is my only C/C++ file
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

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
		
	if (false) {
		execute_kernel();
	}

	/*
	* IMAGE ROTATION DEBUGGING
	*/	
	if (false) {

		std::string texture_map_file_path = THIRD_PARTY_ASSETS_DIRECTORY;
		texture_map_file_path.append(CUBE_MAP_FILE_DIRECTORY);
		
		unsigned char* output_image_data = NULL;
		int x_dimension = 0;
		int y_dimension = 0;
		int implemented_channels = 0;
		int number_of_bytes = 0;

		bool processed_ok = cuda_gl_common.rotate_image_using_cuda(
			texture_map_file_path,
			"posz.png",
			output_image_data,
			135.0f,
			x_dimension,
			y_dimension,
			implemented_channels,
			number_of_bytes
		);		

		processed_ok = cuda_gl_common.write_png_to_disk(
			"posz_rotated.png",
			"posz_rotated.png",
			x_dimension,
			y_dimension,
			implemented_channels,
			(const char*)output_image_data
		);

		delete[] output_image_data;
		output_image_data = nullptr;
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

	// restucture work
	if (true) {
			
		printf("START OF RESTRUCTURE\n");

		window = cuda_gl_common.init_gl(DISPLAY_WIDTH, DISPLAY_HEIGHT);

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}

		code_restructured_scene(window, &cuda_gl_common);
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