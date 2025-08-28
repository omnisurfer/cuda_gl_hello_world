#pragma once

#include <cuda_gl_include.h>
#include <cuda_gl_camera.h>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define SHADER_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/shaders/"
#define ASSETS_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/assets/3d_objects/"
#define THIRD_PARTY_ASSETS_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/Common/assets/"

extern "C" {
	void debug_gl_callback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);

	int execute_image_rotation_kernel(
		const float* input_image_data,
		float* output_image_data,
		int x_dimension,
		int y_dimension,
		int stride,
		float angle_degrees
	);
}

class CUDAGLCommon {

	typedef void (*scene_key_callback_ptr)(GLFWwindow* window, int key, int scancode, int action, int mods);
	typedef void (*scene_mouse_button_callback_ptr)(GLFWwindow* window, int button, int action, int mods);

	scene_key_callback_ptr scene_key_callback_function = nullptr;
	scene_mouse_button_callback_ptr scene_mouse_button_callback_function = nullptr;

public:
	GLuint shader_program;
	std::string vertex_shader_file_path;
	std::string frag_shader_file_path;

public:
		
	void set_opengl_flags() {
		/* opengl configuration */
		glEnable(GL_DEPTH_TEST);	// enable depth-testing
		glDepthFunc(GL_LESS);		// depth-testing interprets a smaller value as "closer"
		glEnable(GL_CULL_FACE);		// cull face
		glCullFace(GL_BACK);		// cull back face
		glFrontFace(GL_CCW);		// GL_CCW for counter clock-wise

		// wire-frame mode
		// glPolygonMode(GL_FRONT, GL_LINE);
		// glPolygonMode(GL_BACK, GL_LINE);
	}

	GLFWwindow* init_gl(int window_width, int window_height);

	void update_shaders(CUDAGLCamera camera);	

	static void error_callback_glfw(int error, const char* description) {
		fprintf(stderr, "GLFW ERROR: code %i msg: %s\n", error, description);
	}

	GLuint compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename);
	
	bool load_texture_into_device_memory(const char* file_name) {

		int x_img_dimension = 0, y_img_dimension = 0, implemented_channels = 0, number_of_bytes = 0;
		unsigned char* image_data = NULL;		
			
		bool image_read_ok = read_in_texture_to_memory(
			file_name,
			image_data,
			x_img_dimension,
			y_img_dimension,
			implemented_channels,
			number_of_bytes
		);
			
		if (!image_read_ok) {
			fprintf(stderr, "ERROR: could not load %s\n", file_name);
			return false;
		}

		// TODO Use CUDA to rotate the image instead of relying on a heavier library like OpenCV or roll my own?
		int total_linear_size = x_img_dimension * y_img_dimension * implemented_channels;

		unsigned char* translated_image_data = new unsigned char[total_linear_size];		
									
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			x_img_dimension,
			y_img_dimension,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image_data
		);	

		free(image_data);
		
		delete[] translated_image_data;
		translated_image_data = nullptr;

		return true;
	}

	bool read_in_texture_to_memory(
		const char* file_name, 
		unsigned char* &image_data,
		int& x_img_dimension,
		int& y_img_dimension,
		int& implemented_channels,
		int& number_of_bytes
	) {

		// LOAD UP IMAGE DATA TBD		
		int force_channels = 4;
		image_data = stbi_load(file_name, &x_img_dimension, &y_img_dimension, &implemented_channels, force_channels);

		if (!image_data) {
			fprintf(stderr, "ERROR: could not load %s\n", file_name);
			return false;
		}
		
		bool is_power_of_two = true;

		// non-power-of-2 dimensions check, i.e. not square
		if ((x_img_dimension & (x_img_dimension - 1)) != 0 || (y_img_dimension & (y_img_dimension - 1)) != 0) {
			fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name);
			is_power_of_two = false;
		}

		number_of_bytes = x_img_dimension * y_img_dimension * sizeof(float);

		return is_power_of_two;
	}

	bool write_png_to_disk(
		std::string file_path, 
		std::string file_name,
		int x_dimension,
		int y_dimension,
		int implemented_channels,
		const char* image_data
	) {
		// TODO need to process file_path
		const char* c_file_name = file_name.c_str();

		/**/
		int status = stbi_write_png(
			c_file_name,
			x_dimension,
			y_dimension,
			implemented_channels,
			image_data,
			y_dimension * implemented_channels
		);
		/**/

		// TODO need to process return

		return true;
	}

	bool rotate_image_using_cuda(
		std::string path_to_files, 
		std::string file_name, 
		unsigned char*& output_image_data,
		const float rotation_angle_degrees,
		int& x_dimension,
		int& y_dimension,
		int& implemented_channels,
		int& number_of_bytes
	) {

		bool processed_ok = true;

		const char* _file_name_and_path = path_to_files.append(file_name).c_str();

		unsigned char* _input_image_data = NULL;

		processed_ok = read_in_texture_to_memory(
			_file_name_and_path,
			_input_image_data,
			x_dimension,
			y_dimension,
			implemented_channels,
			number_of_bytes
		);

		if (processed_ok) {

			// was float, but using implemented channels
			int output_array_size = x_dimension * y_dimension * implemented_channels;

			output_image_data = new unsigned char[number_of_bytes];

			// throw up warning that implemented channels is not equal to designed for stride of 4
			const int kernel_stride = 4;

			if (implemented_channels != kernel_stride) {
				printf(
					"WARNING: Implemented channels %i of image does not match expected kernel stide of %i. Image processing may fail.",
					implemented_channels,
					kernel_stride
				);
			}

			// execute the kernel
			int kernel_run_ok = execute_image_rotation_kernel(
				(const float*)_input_image_data,
				(float*)output_image_data,
				x_dimension,
				y_dimension,
				implemented_channels,
				rotation_angle_degrees
			);

			if (kernel_run_ok > 0) {
				fprintf(stderr, "Image rotation kernel failed to execute.\n");
				processed_ok = false;
			}
		}

		delete[] _input_image_data;
		_input_image_data = nullptr;

		return processed_ok;
	}
};
