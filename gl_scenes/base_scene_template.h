#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>

#include <cuda_gl_setup_utils.h>
#include <cuda_gl_camera.h>
#include <cuda_gl_user_input.h>
#include <cuda_gl_common.h>

#define SPHERE_VERTEX_SHADER_FILE "quat_camera_shader.vert"
#define SPHERE_FRAGMENT_SHADER_FILE "quat_camera_shader.frag"

#define MESH_FILE "3d_objects/sphere.obj"

#define NUM_OF_SPHERS 4

/*
GLuint shader_program;

std::string vertex_shader_file_path;
std::string frag_shader_file_path;
*/

CUDAGLCamera base_temp_camera;
CUDAGLUserInput base_temp_user_input;
CUDAGLCommon base_temp_common;

/*
void update_shaders() {

	glDeleteProgram(shader_program);

	shader_program = compile_and_link_shader_program_from_files(
		vertex_shader_file_path.c_str(), 
		frag_shader_file_path.c_str()
	);

	if (shader_program > 0) {
		glUseProgram(shader_program);
		glUniformMatrix4fv(camera.view_matrix_location, 1, GL_FALSE, camera.view_matrix.m);
		glUniformMatrix4fv(camera.project_matrix_location, 1, GL_FALSE, camera.projection_matrix.m);
	}
}
*/

/* Scene template derived from 06_vcam_with_quaternion */
int base_scene_template(GLFWwindow* window) {
		
	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	/* user input */
	base_temp_user_input.init_cuda_gl_user_input(window);

	/* create camera */
	base_temp_camera.init_camera();

	base_temp_camera.configure_camera(
		0.1f,
		100.0f,
		67.0f,
		window_width,
		window_height
	);

	base_temp_camera.place_camera(vec3(0.0f, 0.0f, 5.0f));

	/* load up spheres */
	vec3 sphere_positions_world[] = {
		vec3(-2.0, 0.0, 0.0),
		vec3(2.0, 0.0, 0.0),
		vec3(-2.0, 0.0, -2.0),
		vec3(1.5, 1.0, -1.0),
	};

	const float sphere_radius = 1.0f;
		
	/* create geometry */
	int model_matrix_location = 0;

	GLfloat* vertex_points = NULL;
	GLfloat* vertex_normals = NULL;
	GLfloat* texture_coordinates = NULL;
	int point_count = 0;

	std::string mesh_file_path = ASSETS_DIRECTORY;
	mesh_file_path.append(MESH_FILE);

	load_obj_file(mesh_file_path.c_str(), vertex_points, texture_coordinates, vertex_normals, point_count);

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

	/* create shaders */
	base_temp_common.vertex_shader_file_path = SHADER_DIRECTORY;
	base_temp_common.vertex_shader_file_path.append(SPHERE_VERTEX_SHADER_FILE);

	base_temp_common.frag_shader_file_path = SHADER_DIRECTORY;
	base_temp_common.frag_shader_file_path.append(SPHERE_FRAGMENT_SHADER_FILE);

	base_temp_common.shader_program = compile_and_link_shader_program_from_files(base_temp_common.vertex_shader_file_path.c_str(), base_temp_common.frag_shader_file_path.c_str());
 	base_temp_camera.view_matrix_location = glGetUniformLocation(base_temp_common.shader_program, "view_matrix");
	base_temp_camera.project_matrix_location = glGetUniformLocation(base_temp_common.shader_program, "projection_matrix");
	model_matrix_location = glGetUniformLocation(base_temp_common.shader_program, "model_matrix");

	if (base_temp_common.shader_program <= 0)
	{	
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}	

	glUseProgram(base_temp_common.shader_program);
	glUniformMatrix4fv(base_temp_camera.view_matrix_location, 1, GL_FALSE, base_temp_camera.view_matrix.m);
	glUniformMatrix4fv(base_temp_camera.project_matrix_location, 1, GL_FALSE, base_temp_camera.projection_matrix.m);
	
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
	
	while (!glfwWindowShouldClose(window))
	{					
		if (base_temp_camera.update_frame(window))
		{
			glfwGetWindowSize(window, &window_width, &window_height);

			/* Check if window was resized */
			base_temp_camera.configure_camera(window_width, window_height);						
			
			/* Clear the drawing sruface */
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);		
			glViewport(0, 0, window_width, window_height);			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
			glUseProgram(base_temp_common.shader_program);

			/* Swap front and back buffers */
			glfwSwapBuffers(window);			
		}

		/* Update camera outside FPS loop */
		// TRANSLATIONS			
		base_temp_user_input.process_movements(&base_temp_camera);
				
		// MOUSE PROCESSING
		if (base_temp_user_input.mouse_button_left) {
				
			// vec3 ray_world = process_mouse(window);
			vec3 ray_world = base_temp_user_input.process_mouse(window, &base_temp_camera);
			
		}

		// PROCESS SHADER RELOAD
		if (base_temp_user_input.reload_shader_key_pressed) {
			base_temp_common.update_shaders(base_temp_camera);
		}

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}