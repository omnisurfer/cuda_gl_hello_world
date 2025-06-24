#pragma once

#include <cuda_gl_include.h>
#include <cuda_gl_camera.h>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define SHADER_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/shaders/"
#define ASSETS_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/assets/"
#define THIRD_PARTY_ASSETS_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/Common/assets/"

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
	
	GLFWwindow* init_gl(int window_width, int window_height);

	void update_shaders(CUDAGLCamera camera);	

	static void error_callback_glfw(int error, const char* description) {
		fprintf(stderr, "GLFW ERROR: code %i msg: %s\n", error, description);
	}

	GLuint compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename);

	/**/
	bool load_texture(const char* file_name) {

		int x, y, n;

		// LOAD UP IMAGE DATA TBD
		int force_channels = 4;
		unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

		if (!image_data) {
			fprintf(stderr, "ERROR: could not load %s\n", file_name);
			return false;
		}

		// non-power-of-2 dimensions check, i.e. not square
		if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
			fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name);
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			x,
			y,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image_data
		);

		free(image_data);

		return true;
	}
	/**/
};
