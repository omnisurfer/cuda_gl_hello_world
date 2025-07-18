#pragma once

#include <cuda_gl_common.h>
#include <cuda_gl_user_input.h>
#include <cuda_gl_camera.h>

#define SPHERE_VERTEX_SHADER_FILE "quat_camera_shader.vert"
#define SPHERE_FRAGMENT_SHADER_FILE "quat_camera_shader.frag"

#define SPHERE_MESH_FILE "3d_objects/sphere.obj"

#define SPHERE_NUM_OF_SPHERS 4

CUDAGLCamera sphere_camera;
CUDAGLUserInput sphere_user_input;

/* taken from 07_ray_picking */
bool ray_sphere_intersect(vec3 ray_origin_world, vec3 ray_direction_world, vec3 sphere_center_world, float sphere_radius, float* intersection_distance) {
	
	vec3 dist_to_sphere = ray_origin_world - sphere_center_world;
	float b = dot(ray_direction_world, dist_to_sphere);
	float c = dot(dist_to_sphere, dist_to_sphere) - sphere_radius * sphere_radius;
	float b_squared_minus_c = b * b - c;

	// check for imaginary answers. Ray completely missies sphere
	if (b_squared_minus_c < 0.0f) {
		return false;
	}

	if (b_squared_minus_c > 0.0f) {
		// get the 2 intersection distances along the ray
		float t_a = -b + sqrt(b_squared_minus_c);
		float t_b = -b - sqrt(b_squared_minus_c);
		*intersection_distance = t_b;

		// if behind viewer, throw one or both away
		if (t_a < 0.0) {
			if (t_b < 0.0) {
				return false;
			}
			else if (t_b < 0.0) {
				*intersection_distance = t_a;
			}
		}

		return true;
	}

	if (b_squared_minus_c == 0.0f) {
		// if behind the viewer
		float t = -b + sqrt(b_squared_minus_c);

		if (t < 0.0f) {
			return false;
		}
		*intersection_distance = t;
		return true;
	}
	// note: check for ray origin is inside sphere radius could occur.
	return false;
}

/* From 06_vcam_with_quaternion */
int draw_quat_cam_spheres(GLFWwindow* window, CUDAGLCommon* cuda_gl_common) {
		
	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	/* user input */
	sphere_user_input.init_cuda_gl_user_input(window);

	/* create camera */
	sphere_camera.init_camera();

	sphere_camera.configure_camera(
		0.1f,
		100.0f,
		67.0f,
		window_width,
		window_height
	);

	sphere_camera.place_camera(vec3(0.0f, 0.0f, 5.0f));

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
	mesh_file_path.append(SPHERE_MESH_FILE);

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
	cuda_gl_common->vertex_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->vertex_shader_file_path.append(SPHERE_VERTEX_SHADER_FILE);

	cuda_gl_common->frag_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->frag_shader_file_path.append(SPHERE_FRAGMENT_SHADER_FILE);

	cuda_gl_common->shader_program = cuda_gl_common->compile_and_link_shader_program_from_files(cuda_gl_common->vertex_shader_file_path.c_str(), cuda_gl_common->frag_shader_file_path.c_str());

 	sphere_camera.view_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "view_matrix");
	sphere_camera.project_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "projection_matrix");
	
	model_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "model_matrix");
	int blue_frag_channel_location = glGetUniformLocation(cuda_gl_common->shader_program, "blue_frag_channel");

	if (cuda_gl_common->shader_program <= 0)
	{	
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}	

	glUseProgram(cuda_gl_common->shader_program);
	glUniformMatrix4fv(sphere_camera.view_matrix_location, 1, GL_FALSE, sphere_camera.view_matrix.m);
	glUniformMatrix4fv(sphere_camera.project_matrix_location, 1, GL_FALSE, sphere_camera.projection_matrix.m);
	
	mat4 model_matrices[SPHERE_NUM_OF_SPHERS];
	for (int i = 0; i < SPHERE_NUM_OF_SPHERS; i++) {
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
	
	int selected_sphere = -1;

	while (!glfwWindowShouldClose(window))
	{					
		if (sphere_camera.update_frame(window))
		{
			glfwGetWindowSize(window, &window_width, &window_height);

			/* Check if window was resized */
			sphere_camera.configure_camera(window_width, window_height);						
			
			/* Clear the drawing sruface */
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);		
			glViewport(0, 0, window_width, window_height);			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
			glUseProgram(cuda_gl_common->shader_program);

			// color selected spheres
			for (int i = 0; i < SPHERE_NUM_OF_SPHERS; i++) {
				
				if (selected_sphere == i) {					
					glUniform1f(blue_frag_channel_location, 1.0f);					
				}
				else {
					glUniform1f(blue_frag_channel_location, 0.0f);
				}

				glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrices[i].m);
				glDrawArrays(GL_TRIANGLES, 0, point_count);
			}

			/* Swap front and back buffers */
			glfwSwapBuffers(window);			
		}

		/* Update camera outside FPS loop */
		// TRANSLATIONS			
		sphere_user_input.process_movements(&sphere_camera);
				
		// MOUSE PROCESSING
		if (sphere_user_input.mouse_button_left) {
				
			// vec3 ray_world = process_mouse(window);
			vec3 ray_world = sphere_user_input.process_mouse(window, &sphere_camera);

			// sphere check
			int closest_sphere_clicked = -1;
			float closest_intersection = 0.0f;

			for (int i = 0; i < SPHERE_NUM_OF_SPHERS; i++) {
				float t_distance = 0.0f;

				if (ray_sphere_intersect(sphere_camera.camera_position, ray_world, sphere_positions_world[i], sphere_radius, &t_distance)) {

					// only use cloest sphere
					if (closest_sphere_clicked == -1 || t_distance < closest_intersection) {
						closest_sphere_clicked = i;
						closest_intersection = t_distance;
					}
				}
			}
			selected_sphere = closest_sphere_clicked;
			// printf("sphere %i was selected\n", selected_sphere);
		}

		// PROCESS SHADER RELOAD
		if (sphere_user_input.reload_shader_key_pressed) {
			cuda_gl_common->update_shaders(sphere_camera);
		}

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}