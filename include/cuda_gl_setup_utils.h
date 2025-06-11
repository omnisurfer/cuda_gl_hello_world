#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <vector>

#define SHADER_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/shaders/"
#define ASSETS_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/assets/"
#define THIRD_PARTY_ASSETS_DIRECTORY "D:/DevSync/Sandbox/CUDA/cuda_gl_hello_world/Common/assets/"

GLFWwindow* init_gl(int window_width, int window_height);

typedef void (*scene_key_callback_ptr)(GLFWwindow* window, int key, int scancode, int action, int mods);
typedef void (*scene_mouse_button_callback_ptr)(GLFWwindow* window, int button, int action, int mods);

void set_scene_key_callback_function(scene_key_callback_ptr scene_key_callback);
void set_scene_mouse_button_callback_function(scene_mouse_button_callback_ptr scene_mouse_callback);

void gl_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void gl_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void error_callback_glfw(int error, const char* description);

GLuint compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename);

