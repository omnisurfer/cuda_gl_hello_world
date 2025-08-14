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
				
        // TODO: Clean this up
		std::string texture_map_file_path = THIRD_PARTY_ASSETS_DIRECTORY;
		texture_map_file_path.append(CUBE_MAP_FILE_DIRECTORY);
		
		const char* file_name = texture_map_file_path.append("posz.png").c_str();

		int x_img_dimension = 0;	
		int y_img_dimension = 0;
		int implemented_channels = 0;
		int number_of_bytes = 0;
		const int stride = 4;		// 4 bytes per channel, RGBA, a float		
		const float rotation_angle_degrees = 138.0;

		unsigned char* input_image_data = NULL;
		unsigned char* output_image_data = NULL;

		bool image_read_ok = cuda_gl_common.read_in_texture_to_memory(
			file_name,
			input_image_data,
			x_img_dimension,
			y_img_dimension,
			implemented_channels,
			number_of_bytes
		);

		int output_array_size = x_img_dimension * y_img_dimension * sizeof(float);

		output_image_data = new unsigned char[number_of_bytes];

		int success = execute_image_rotation_kernel(
			(const float*)input_image_data,
			(float*)output_image_data,
			x_img_dimension,
			y_img_dimension,
			stride,
			rotation_angle_degrees
		);

		if (success > 0) {
			fprintf(stderr, "Image rotation kernel failed to execute.\n");
		}

		std::string _file_name = "rotate_output_angle_" + std::to_string(rotation_angle_degrees) + "deg.png";
		const char* c_file_name = _file_name.c_str();

		// Write out the image
		if (true) {
			/*
			stbi_write_png(
				c_file_name,
				x_img_dimension,
				y_img_dimension,
				implemented_channels,
				output_image_data,
				y_img_dimension * implemented_channels
			);
			*/
		}

		delete[] output_image_data;
		output_image_data = nullptr;
	}

	if (true) {

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
			90.0f,
			x_dimension,
			y_dimension,
			implemented_channels,
			number_of_bytes
		);

		printf("TEST\n");

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