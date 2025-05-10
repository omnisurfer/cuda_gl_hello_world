#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>

#include "cuda_gl_setup_utils.h"
#include "cuda_gl_scene_utils.h"

/* https://antongerdelan.net/opengl/hellotriangle.html */
int draw_q_camera_triangle(GLFWwindow* window) {

	/* Render a triangle */
	/**/
	GLfloat triangle_points[] = {
		0.0f, 0.5f, 0.1f,
		0.5f, -0.5f, 0.1f,
		-0.5f, -0.5f, 0.1f
	};

	GLfloat triangle_colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	/**/

	mat4 model_matrix = {
		0.0f, 5.5f, 1.1f, 0.0f,
		5.5f, -5.5f, 1.1f, 0.0f,
		-5.5f, -5.5f, 1.1f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
	};

	int number_of_triangle_points = sizeof(triangle_points) / sizeof(triangle_points)[0];
	int number_of_colors_points = sizeof(triangle_colors) / sizeof(triangle_colors)[0];

	printf("t points %i, t colors %i\n", number_of_triangle_points, number_of_colors_points);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"

	// triangle 1 memory resources
	/**/
	GLuint points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, number_of_triangle_points * sizeof(GLfloat), triangle_points, GL_STATIC_DRAW);
	/**/

	/**/
	GLuint triangle_vao = 0;
	glGenVertexArrays(1, &triangle_vao);
	glBindVertexArray(triangle_vao);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	/**/

	mat4 view_matrix;
	mat4 projection_matrix;
	vec3 camera_position(0.0f, 0.0f, 5.0f);
	vec3 sphere_positions_world[] = {
		vec3(-2.0, 0.0, 0.0),
		vec3(2.0, 0.0, 0.0),
		vec3(-2.0, 0.0, -2.0),
		vec3(1.5, 1.0, -1.0),
	};

	GLuint shader_program = compile_and_link_shader_program_from_files("shaders\\q_camera_shader.vert", "shaders\\q_camera_shader.frag");

	int model_matrix_location = glGetUniformLocation(shader_program, "model_matrix");
	int view_matrix_location = glGetUniformLocation(shader_program, "view_matrix");
	int project_matrix_location = glGetUniformLocation(shader_program, "projection_matrix");

	if (shader_program > 0)
	{
		glUseProgram(shader_program);
		glBindVertexArray(triangle_vao);
	}
	else {
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}

	// create camera
	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	view_matrix = create_camera(window_width, window_height);

	glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.m);
	glUniformMatrix4fv(project_matrix_location, 1, GL_FALSE, projection_matrix.m);

	/* 0 swap immediate 1 sync to monitor */
	glfwSwapInterval(1);

	int degree_step = 0;
	const double fps_limit_frequency = 60.0;
	const double fps_limit_period = 1 / fps_limit_frequency;
	double last_update_time = 0;
	double last_frame_time = 0;

	// glEnable(GL_CULL_FACE); // cull face
	// glCullFace(GL_BACK);    // cull back face
	glFrontFace(GL_CW);     // GL_CCW for counter clock-wise

	while (!glfwWindowShouldClose(window))
	{

		// Code to limit framerate
		double now = glfwGetTime();
		double delta_time = now - last_update_time;

		if ((now - last_frame_time) >= fps_limit_period)
		{
			/* Check if window was resized */			
			glfwGetWindowSize(window, &window_width, &window_height);
			glViewport(0, 0, window_width, window_height);

			/* Clear the drawing sruface */
			glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			double current_time = glfwGetTime();			
			
			glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrix.m);

			// wire-frame mode
			// glPolygonMode(GL_FRONT, GL_LINE);
			// glPolygonMode(GL_BACK, GL_LINE);

			int number_of_triangle_vertices = number_of_colors_points / 3;

			glDrawArrays(GL_TRIANGLES, 0, number_of_triangle_vertices);
			/**/

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* calucate fps */
			double fps = 1 / (now - last_frame_time);
			char temp[256];
			sprintf_s(temp, "FPS %.2lf", fps);
			glfwSetWindowTitle(window, temp);

			last_frame_time = now;
		}

		last_update_time = now;

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;

}