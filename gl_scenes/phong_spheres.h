#pragma once

#include <cuda_gl_common.h>
#include <cuda_gl_user_input.h>
#include <cuda_gl_camera.h>
#include <cuda_gl_lighting.h>

#define PHONG_VERTEX_SHADER_FILE "phong_shader.vert"
#define PHONG_FRAGMENT_SHADER_FILE "phong_shader.frag"

#define PHONG_MESH_FILE "3d_objects/sphere.obj"

#define PHONG_NUM_OF_SPHERS 4

CUDAGLCamera phong_camera;
CUDAGLUserInput phong_user_input;

/* taken from 07_ray_picking */
bool phong_ray_sphere_intersect(vec3 ray_origin_world, vec3 ray_direction_world, vec3 sphere_center_world, float sphere_radius, float* intersection_distance) {
	
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
int draw_phong_spheres(GLFWwindow* window, CUDAGLCommon* cuda_gl_common) {
		
	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	/* user input */
	phong_user_input.init_cuda_gl_user_input(window);

	/* create camera */
	phong_camera.init_camera();

	phong_camera.configure_camera(
		0.1f,
		100.0f,
		67.0f,
		window_width,
		window_height
	);

	phong_camera.place_camera(vec3(0.0f, 0.0f, 10.0f));

	/* load up spheres */
	vec3 sphere_positions_world[] = {
		vec3(-2.0, 0.0, 0.0),
		vec3(2.0, 0.0, 0.0),
		vec3(-2.0, 0.0, -2.0),
		vec3(1.5, 1.0, -1.0),
	};

	const int number_of_lights = 2;

	Light lights[number_of_lights];

	lights[0].light_position_world = vec3(0.0, 0.0, 5.0);
	lights[0].Ls = vec3(0.2, 0.2, 0.2);
	lights[0].Ld = vec3(0.7, 0.7, 0.7);
	lights[0].La = vec3(0.1, 0.1, 0.1);

	lights[0].Ks = vec3(1.0, 1.0, 1.0);
	lights[0].Kd = vec3(1.0, 0.0, 0.0);
	lights[0].Ka = vec3(1.0, 1.0, 1.0);
	lights[0].specular_exponent = 100.0;
	
	/**/
	lights[1].light_position_world = vec3(0.0, 0.0, -5.0);
	lights[1].Ls = vec3(0.2, 0.2, 0.2);
	lights[1].Ld = vec3(0.7, 0.7, 0.7);
	lights[1].La = vec3(0.1, 0.1, 0.1);

	lights[1].Ks = vec3(1.0, 1.0, 1.0);
	lights[1].Kd = vec3(0.0, 0.25, 0.0);
	lights[1].Ka = vec3(1.0, 1.0, 1.0);
	lights[1].specular_exponent = 100.0;
	/**/

	const float sphere_radius = 1.0f;
		
	/* create geometry */
	int model_matrix_location = 0;

	GLfloat* vertex_points = NULL;
	GLfloat* vertex_normals = NULL;
	GLfloat* texture_coordinates = NULL;
	int point_count = 0;

	std::string mesh_file_path = ASSETS_DIRECTORY;
	mesh_file_path.append(PHONG_MESH_FILE);

	load_obj_file(mesh_file_path.c_str(), vertex_points, texture_coordinates, vertex_normals, point_count);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint points_vbo;
	GLuint normals_vbo;
	if (NULL != vertex_points) {

		// vertex points
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), vertex_points, GL_STATIC_DRAW);		
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		// vertex normals
		glGenBuffers(1, &normals_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * point_count * sizeof(GLfloat), vertex_normals, GL_STATIC_DRAW);
		
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);		
		glEnableVertexAttribArray(1);
	}

	/* create shaders */
	cuda_gl_common->vertex_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->vertex_shader_file_path.append(PHONG_VERTEX_SHADER_FILE);

	cuda_gl_common->frag_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->frag_shader_file_path.append(PHONG_FRAGMENT_SHADER_FILE);

	cuda_gl_common->shader_program = cuda_gl_common->compile_and_link_shader_program_from_files(cuda_gl_common->vertex_shader_file_path.c_str(), cuda_gl_common->frag_shader_file_path.c_str());

 	phong_camera.view_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "view_matrix");
	phong_camera.project_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "projection_matrix");
	
	model_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "model_matrix");
	int blue_frag_channel_location = glGetUniformLocation(cuda_gl_common->shader_program, "blue_frag_channel");	

	if (cuda_gl_common->shader_program <= 0)
	{	
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}

	glUseProgram(cuda_gl_common->shader_program);
	glUniformMatrix4fv(phong_camera.view_matrix_location, 1, GL_FALSE, phong_camera.view_matrix.m);
	glUniformMatrix4fv(phong_camera.project_matrix_location, 1, GL_FALSE, phong_camera.projection_matrix.m);

	/* setup lights */
	GLuint light_block_location = glGetUniformBlockIndex(cuda_gl_common->shader_program, "light_source");
	glUniformBlockBinding(cuda_gl_common->shader_program, light_block_location, 0);


	/* https://community.khronos.org/t/sending-an-array-of-structs-to-shader-via-an-uniform-buffer-object/75092 */
	/* https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf */
	GLuint lights_vbo;
	glGenBuffers(1, &lights_vbo);
	glBindBuffer(GL_UNIFORM_BUFFER, lights_vbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * number_of_lights, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, light_block_location, lights_vbo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, lights_vbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lights), lights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	/* setup lights */

	printf("size Light %llu lights %llu lights[] %llu\n", sizeof(Light), sizeof(lights), sizeof(lights[0]));
	
	mat4 model_matrices[PHONG_NUM_OF_SPHERS];
	for (int i = 0; i < PHONG_NUM_OF_SPHERS; i++) {
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
		if (phong_camera.update_frame(window))
		{
			glfwGetWindowSize(window, &window_width, &window_height);

			/* Check if window was resized */
			phong_camera.configure_camera(window_width, window_height);						
			
			/* Clear the drawing sruface */
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);		
			glViewport(0, 0, window_width, window_height);			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
			glUseProgram(cuda_gl_common->shader_program);

			// color selected spheres
			for (int i = 0; i < PHONG_NUM_OF_SPHERS; i++) {
				
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
		phong_user_input.process_movements(&phong_camera);
				
		// MOUSE PROCESSING
		if (phong_user_input.mouse_button_left) {
				
			// vec3 ray_world = process_mouse(window);
			vec3 ray_world = phong_user_input.process_mouse(window, &phong_camera);

			// sphere check
			int closest_sphere_clicked = -1;
			float closest_intersection = 0.0f;

			for (int i = 0; i < PHONG_NUM_OF_SPHERS; i++) {
				float t_distance = 0.0f;

				if (phong_ray_sphere_intersect(phong_camera.camera_position, ray_world, sphere_positions_world[i], sphere_radius, &t_distance)) {

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
		if (phong_user_input.reload_shader_key_pressed) {
			cuda_gl_common->update_shaders(phong_camera);
		}

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}