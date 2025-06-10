#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define CUBE_VERTEX_SHADER_FILE "cube_shader.vert"
#define CUBE_FRAGMENT_SHADER_FILE "cube_shader.frag"

#define CUBE_MAP_FILE_DIRECTORY "cube_maps/Yokohama3/"

bool load_cube_map_side(
	GLuint texture,
	GLenum side_target,
	const char* file_name)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

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
		side_target,
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

/* https://antongerdelan.net/opengl/cubemaps.html */
void create_cube_map(
	const char* front,
	const char* back,
	const char* top,
	const char* bottom,
	const char* left,
	const char* right,
	GLuint* texture_cube) {

	// generate a cube-map texture to hold all the sides
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, texture_cube);

	// load each image
	bool success = load_cube_map_side(*texture_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);
	success = load_cube_map_side(*texture_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
	success = load_cube_map_side(*texture_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
	success = load_cube_map_side(*texture_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
	success = load_cube_map_side(*texture_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, left);
	success = load_cube_map_side(*texture_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, right);

	// format cube map texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

int draw_cube_map(GLFWwindow* window) {

	GLfloat cube_points[] = {
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f,  10.0f
	};

	int points_per_triangle = 9;
	int triangles_per_face = 2;
	int faces_per_cube = 6;

	int points_per_cube = points_per_triangle * triangles_per_face * faces_per_cube;

	GLuint cube_points_vbo;
	glGenBuffers(1, &cube_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points), &cube_points, GL_STATIC_DRAW);

	GLuint cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, cube_points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint dummy_texture_cube;

	std::string cube_map_file_path = THIRD_PARTY_ASSETS_DIRECTORY;
	cube_map_file_path.append(CUBE_MAP_FILE_DIRECTORY);

	create_cube_map(
		std::string(cube_map_file_path).append("posz.jpg").c_str(),
		std::string(cube_map_file_path).append("negz.jpg").c_str(),
		std::string(cube_map_file_path).append("posy.jpg").c_str(),
		std::string(cube_map_file_path).append("negy.jpg").c_str(),
		std::string(cube_map_file_path).append("posx.jpg").c_str(),
		std::string(cube_map_file_path).append("negx.jpg").c_str(),
		&dummy_texture_cube
	);

	std::string vertex_shader_file_path = SHADER_DIRECTORY;
	vertex_shader_file_path.append(CUBE_VERTEX_SHADER_FILE);

	std::string frag_shader_file_path = SHADER_DIRECTORY;
	frag_shader_file_path.append(CUBE_FRAGMENT_SHADER_FILE);

	GLuint shader_program = compile_and_link_shader_program_from_files(vertex_shader_file_path.c_str(), frag_shader_file_path.c_str());	

	if (shader_program > 0)
	{
		glUseProgram(shader_program);
		glBindVertexArray(cube_vao);
	}

	glUseProgram(shader_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dummy_texture_cube);
	glBindVertexArray(cube_vao);



	while (!glfwWindowShouldClose(window))
	{
		glDepthMask(GL_FALSE);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
