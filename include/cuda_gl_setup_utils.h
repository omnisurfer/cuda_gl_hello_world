#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <vector>

GLFWwindow* init_gl(int window_width, int window_height);

void gl_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void error_callback_glfw(int error, const char* description);

GLuint compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename);

