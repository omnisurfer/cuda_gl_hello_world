#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <vector>

static void error_callback_glfw(int error, const char* description);

static void gl_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLuint compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename);