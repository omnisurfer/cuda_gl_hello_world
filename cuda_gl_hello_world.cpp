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
		
	if (true) {
		execute_kernel();
	}

	/*
	* IMAGE ROTATION DEBUGGING
	*/
	if (true) {
				
        // TODO: Clean this up
		std::string texture_map_file_path = THIRD_PARTY_ASSETS_DIRECTORY;
		texture_map_file_path.append(CUBE_MAP_FILE_DIRECTORY);
		
		const char* file_name = texture_map_file_path.append("posz.png").c_str();

		int x_img_dimension = 0;	
		int y_img_dimension = 0;
		int implemented_channels = 0;
		int number_of_bytes = 0;
		const int stride = 4;		// 4 bytes per channel, RGBA, a float		
		const float rotation_angle_degrees = 77.0;
		// const int image_data_bytes_size = sizeof(float) * x_dimension * y_dimension;

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

		// print results for debugging
		if (false) {
			for (int i = 0; i < x_img_dimension * y_img_dimension; i++) {

				if (i > 256)
					break;

				printf("%i, %f, %f\n", i, (float)input_image_data[i], (float)output_image_data[i]);
			}
		}

		// Write out the image
		stbi_write_png(
			"output_77.png",
			x_img_dimension, 
			y_img_dimension, 
			implemented_channels, 
			output_image_data, 
			y_img_dimension * implemented_channels
		);

		delete[] output_image_data;
		output_image_data = nullptr;
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