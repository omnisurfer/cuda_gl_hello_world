#pragma once

#include <cuda_gl_setup_utils.h>
// #include <cuda_gl_scene_utils.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>

/* https://antongerdelan.net/opengl/hellotriangle.html */
int draw_simple_triangle(GLFWwindow* window) {

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

	// column order looks like row because OpenGL will read as a 1d array with column 1 occupying [0..3] and so on.
	// If the 1d array represents data in row order, you must call glUniform with transposition set to GL_TRUE.
	/*
	* a b c d
	* e f g h
	* i j k l
	* m n o p
	*/
	float translation_matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f, // first column - a e i m
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.0f, 0.0f, 1.0f, // fourth column - d h l p
	};

	// translate_matrix(translation_matrix, 0.5, 0.0, 0.0);

	float rotation_matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f, // first column - a e i m
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, // fourth column - d h l p
	};
	/**/

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

	GLuint colors_vbo = 0;
	glGenBuffers(1, &colors_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, number_of_colors_points * sizeof(GLfloat), triangle_colors, GL_STATIC_DRAW);
	/**/

	/**/
	GLuint triangle_vao = 0;
	glGenVertexArrays(1, &triangle_vao);
	glBindVertexArray(triangle_vao);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	/**/
	
	std::string vertex_shader_file_path = SHADER_DIRECTORY;
	vertex_shader_file_path.append("triangle_shader.vert");

	std::string frag_shader_file_path = SHADER_DIRECTORY;
	frag_shader_file_path.append("triangle_shader.frag");

	GLuint shader_program = compile_and_link_shader_program_from_files(vertex_shader_file_path.c_str(), frag_shader_file_path.c_str());

	int translation_matrix_location = glGetUniformLocation(shader_program, "translation_matrix");
	int rotation_matrix_location = glGetUniformLocation(shader_program, "rotation_matrix");

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

	glUniformMatrix4fv(translation_matrix_location, 1, GL_FALSE, translation_matrix);
	glUniformMatrix4fv(rotation_matrix_location, 1, GL_FALSE, rotation_matrix);

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
			int window_width, window_height;
			glfwGetWindowSize(window, &window_width, &window_height);
			glViewport(0, 0, window_width, window_height);

			/* Clear the drawing sruface */
			glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			double current_time = glfwGetTime();
			/* Animation */
			/**/
			float amplitude = 1.0;
			float frequency = 1.0;
			float rad_step = float(2 * M_PI / 180.0f) * degree_step;
			float period = 1 / frequency;

			int degree_step_limit = int(fps_limit_frequency / frequency) * 360;

			float phase = 90.0;
			float phase_rad = float(2 * M_PI / 180.0f) * phase;

			float theta = rad_step * frequency;
			float theta_phase = rad_step * frequency + phase_rad;

			float sin_wave = amplitude * sin(theta);
			float sin_wave_out_of_phase = amplitude * sin(theta_phase);

			int cycle_limit = int(fps_limit_frequency / frequency);

			if (degree_step < degree_step_limit) {
				degree_step++;
			}
			else
				degree_step = 0;

			// update translation matrix
			translation_matrix[12] = sin_wave;
			// translate_matrix(translation_matrix, sin_wave, sin_wave, 0.0);

			// update rotation matrix on z axis - roll?
			/**/
			rotation_matrix[0] = cos(theta_phase);
			rotation_matrix[1] = sin(theta_phase);
			rotation_matrix[4] = -sin(theta_phase);
			rotation_matrix[5] = cos(theta_phase);
			/**/

			// update rotation matrix on x axis - pitch?
			/**/
			rotation_matrix[5] = cos(theta);
			rotation_matrix[6] = sin(theta);
			rotation_matrix[9] = -sin(theta);
			rotation_matrix[10] = cos(theta);
			/**/

			// update rotation matrix on y axis - yaw?
			/**/
			rotation_matrix[0] = cos(theta_phase);
			rotation_matrix[2] = -sin(theta_phase);
			rotation_matrix[8] = sin(theta_phase);
			rotation_matrix[10] = cos(theta_phase);
			/**/

			glUniformMatrix4fv(translation_matrix_location, 1, GL_FALSE, translation_matrix);
			// glUniformMatrix4fv(rotation_matrix_location, 1, GL_FALSE, rotation_matrix);

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