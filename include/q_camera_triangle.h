#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>

#include "cuda_gl_setup_utils.h"
#include "cuda_gl_scene_utils.h"

#define VERTEX_SHADER_FILE "shaders\\q_camera_shader.vert"
#define FRAGMENT_SHADER_FILE "shaders\\q_camera_shader.frag"

#define MESH_FILE "3d_objects/sphere.obj"
#define NUM_OF_SPHERS 4

/* Derived from 06_vcam_with_quaternion */
int draw_q_camera_triangle(GLFWwindow* window) {

	/* create camera */
	mat4 view_matrix;
	mat4 projection_matrix;
	vec3 camera_position(0.0f, 0.0f, 5.0f);
	float camera_heading = 0.0f;
	vec3 sphere_positions_world[] = {
		vec3(-2.0, 0.0, 0.0),
		vec3(2.0, 0.0, 0.0),
		vec3(-2.0, 0.0, -2.0),
		vec3(1.5, 1.0, -1.0),
	};
	
	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	configure_camera(
		0.1f,
		100.0f,
		67.0f,
		window_width,
		window_height,
		&projection_matrix
	);

	view_matrix = place_camera(camera_position, camera_heading);

	/* create geometry */
	GLfloat* vertex_points = NULL;
	GLfloat* vertex_normals = NULL;
	GLfloat* texture_coordinates = NULL;
	int point_count = 0;
	load_obj_file(MESH_FILE, vertex_points, texture_coordinates, vertex_normals, point_count);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint points_vbo;
	if (NULL != vertex_points) {
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), vertex_points, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}

	/* Create shaders */
	GLuint shader_program = compile_and_link_shader_program_from_files(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
	int model_matrix_location = glGetUniformLocation(shader_program, "model_matrix");
	int view_matrix_location = glGetUniformLocation(shader_program, "view_matrix");
	int project_matrix_location = glGetUniformLocation(shader_program, "projection_matrix");

	if (shader_program <= 0)
	{	
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}	

	glUseProgram(shader_program);
	glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.m);
	glUniformMatrix4fv(project_matrix_location, 1, GL_FALSE, projection_matrix.m);
	
	mat4 model_matrices[NUM_OF_SPHERS];
	for (int i = 0; i < NUM_OF_SPHERS; i++) {
		model_matrices[i] = translate(identity_mat4(), sphere_positions_world[i]);
	}


	/* opengl configuration */
	glEnable(GL_DEPTH_TEST);	// enable depth-testing
	glEnable(GL_LESS);			// depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE);		// cull face
	glCullFace(GL_BACK);		// cull back face
	glFrontFace(GL_CCW);		// GL_CCW for counter clock-wise

	// wire-frame mode
	// glPolygonMode(GL_FRONT, GL_LINE);
	// glPolygonMode(GL_BACK, GL_LINE);

	/* 0 swap immediate 1 sync to monitor */
	glfwSwapInterval(1);

	int degree_step = 0;
	const double fps_limit_frequency = 60.0;
	const double fps_limit_period = 1 / fps_limit_frequency;
	double last_update_time = 0;
	double last_frame_time = 0;

	while (!glfwWindowShouldClose(window))
	{

		// Code to limit framerate
		double now = glfwGetTime();
		double delta_time = now - last_update_time;

		if ((now - last_frame_time) >= fps_limit_period)
		{									
			glfwGetWindowSize(window, &window_width, &window_height);

			// TODO need to create a method that does not change the Rotation matrix...
			if (false) {
				configure_camera(
					0.1f,
					100.0f,
					67.0f,
					window_width,
					window_height,
					&projection_matrix
				);
			}

			/* Check if window was resized */
			/* Clear the drawing sruface */
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
			glViewport(0, 0, window_width, window_height);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			double current_time = glfwGetTime();			
			
			glUseProgram(shader_program);
			for (int i = 0; i < NUM_OF_SPHERS; i++) {
				glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrices[i].m);
				glDrawArrays(GL_TRIANGLES, 0, point_count);
			}

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* calucate fps */
			double fps = 1 / (now - last_frame_time);
			char temp[256];
			sprintf_s(temp, "FPS %.2lf", fps);
			glfwSetWindowTitle(window, temp);

			last_frame_time = now;			
		}

		/* Update camera outside FPS loop */

		bool camera_moved = false;
		// x y z
		vec3 move(0.0f, 0.0f, 0.0f);

		// r p y
		vec3 rotate(0.0f, 0.0f, 0.0f);
						
		float camera_speed = 5.0f; // 1 unit per second?
		float camera_heading_speed = 100.0f; //30 degrees per second
		float camera_heading = 0.0f; // y-rotation in degrees

		// Displacement
		if (glfwGetKey(window, GLFW_KEY_W)) {
			move.v[2] -= (camera_speed * delta_time);
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			move.v[0] -= (camera_speed * delta_time);
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			move.v[2] += (camera_speed * delta_time);
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			move.v[0] += (camera_speed * delta_time);
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_Q)) {
			move.v[1] += (camera_speed * delta_time);
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_E)) {
			move.v[1] -= (camera_speed * delta_time);
			camera_moved = true;
		}
		// Rotation
		// Roll
		if (glfwGetKey(window, GLFW_KEY_Z)) {
			rotate.v[0] += camera_heading_speed * delta_time;			
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_X)) {
			rotate.v[0] -= camera_heading_speed * delta_time;
			camera_moved = true;
		}

		// Pitch
		if (glfwGetKey(window, GLFW_KEY_UP)) {
			rotate.v[1] += camera_heading_speed * delta_time;
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			rotate.v[1] -= camera_heading_speed * delta_time;
			camera_moved = true;
		}
		
		// Yaw
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {			
			rotate.v[2] += camera_heading_speed * delta_time;
			// printf("cam_yaw %f speed %f delta %f\n",rotate.v[2], camera_heading_speed, delta_time);
			camera_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			rotate.v[2] -= camera_heading_speed * delta_time;
			camera_moved = true;
		}

		if (camera_moved) {

			view_matrix = move_camera(move, rotate);
			glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.m);

		}

		last_update_time = now;

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;

}