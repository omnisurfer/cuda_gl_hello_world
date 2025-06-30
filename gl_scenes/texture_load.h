#pragma once

#include <cuda_gl_common.h>
#include <cuda_gl_user_input.h>
#include <cuda_gl_camera.h>
#include <cuda_gl_lighting.h>

#define TEXTURE_VERTEX_SHADER_FILE "texture_orig_shader.vert"
#define TEXTURE_FRAGMENT_SHADER_FILE "texture_orig_shader.frag"

#define TEXTURE_MESH_FILE "3d_objects/sphere.obj"

#define CUBE_MAP_FILE_DIRECTORY "cube_maps/Yokohama3/"

#define TEXTURE_NUM_OF_SPHERES 4

CUDAGLCamera texture_load_camera;
CUDAGLUserInput texture_load_user_input;

GLuint lighting_shader_program = 0;
GLuint texture_shader_program = 0;

/* taken from 07_ray_picking */
bool texture_load_ray_intersect(vec3 ray_origin_world, vec3 ray_direction_world, vec3 sphere_center_world, float sphere_radius, float* intersection_distance) {
	
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

/* From 09_texture_load */
int draw_texture_load(GLFWwindow* window, CUDAGLCommon* cuda_gl_common) {
		
	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	/* user input */
	texture_load_user_input.init_cuda_gl_user_input(window);

	/* create camera */
	texture_load_camera.init_camera();

	texture_load_camera.configure_camera(
		0.1f,
		100.0f,
		67.0f,
		window_width,
		window_height
	);

	texture_load_camera.place_camera(vec3(0.0f, 0.0f, 10.0f));
	
	const int number_of_lights = 3;

	Light lights[number_of_lights];

	lights[0].light_position_world = vec4(0.0, 0.0, 5.0, 1.0);
	lights[0].Ls = vec4(0.2, 0.2, 0.2, 1.0);
	lights[0].Ld = vec4(0.7, 0.7, 0.7, 1.0);
	lights[0].La = vec4(0.1, 0.1, 0.1, 1.0);

	lights[0].Ks = vec4(1.0, 1.0, 1.0, 1.0);
	lights[0].Kd = vec4(1.0, 0.0, 0.0, 1.0);
	lights[0].Ka = vec4(1.0, 1.0, 1.0, 1.0);
	lights[0].specular_exponent = 100.0;
	
	/**/
	lights[1].light_position_world = vec4(0.0, 0.0, -5.0, 1.0);
	lights[1].Ls = vec4(0.2, 0.2, 0.2, 1.0);
	lights[1].Ld = vec4(0.7, 0.7, 0.7, 1.0);
	lights[1].La = vec4(0.1, 0.1, 0.1, 1.0);

	lights[1].Ks = vec4(1.0, 1.0, 1.0, 1.0);
	lights[1].Kd = vec4(0.0, 1.0, 0.0, 1.0);
	lights[1].Ka = vec4(1.0, 1.0, 1.0, 1.0);
	lights[1].specular_exponent = 100.0;

	lights[2].light_position_world = vec4(0.0, 5.0, 0.0, 1.0);
	lights[2].Ls = vec4(0.2, 0.2, 0.2, 1.0);
	lights[2].Ld = vec4(0.7, 0.7, 0.7, 1.0);
	lights[2].La = vec4(0.1, 0.1, 0.1, 1.0);

	lights[2].Ks = vec4(1.0, 1.0, 1.0, 1.0);
	lights[2].Kd = vec4(0.0, 0.0, 1.0, 1.0);
	lights[2].Ka = vec4(1.0, 1.0, 1.0, 1.0);
	lights[2].specular_exponent = 100.0;
	/**/

	const float sphere_radius = 1.0f;
	
#pragma region Sphere Geometry
	/* geometry */
	vec3 model_positions_world[] = {
		vec3(-2.0, 0.0, 0.0),
		vec3(2.0, 0.0, 0.0),
		vec3(-2.0, -1.0, -2.5),
		vec3(2.0, 1.0, -2.5),
		vec3(1.0, 1.0, 0.0),
	};

	int model_matrix_location = 0;

	GLfloat* vertex_points = NULL;
	GLfloat* vertex_normals = NULL;
	GLfloat* texture_coordinates = NULL;
	int point_count = 0;

	std::string mesh_file_path = ASSETS_DIRECTORY;
	mesh_file_path.append(TEXTURE_MESH_FILE);

	load_obj_file(mesh_file_path.c_str(), vertex_points, texture_coordinates, vertex_normals, point_count);
	
	GLuint vao_spheres;
	glGenVertexArrays(1, &vao_spheres);
	glBindVertexArray(vao_spheres);
		
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
	else {
		fprintf(stderr, "ERROR: could not load meshes.");
		glfwTerminate();
		return 0;
	}
	
	mat4 model_matrices[TEXTURE_NUM_OF_SPHERES + 1];
	for (int i = 0; i < TEXTURE_NUM_OF_SPHERES; i++) {
		model_matrices[i] = translate(identity_mat4(), model_positions_world[i]);
	}
#pragma endregion

#pragma region Sphere Shaders
	/* lighting shaders */
	cuda_gl_common->vertex_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->vertex_shader_file_path.append(TEXTURE_VERTEX_SHADER_FILE);

	cuda_gl_common->frag_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->frag_shader_file_path.append(TEXTURE_FRAGMENT_SHADER_FILE);

	cuda_gl_common->shader_program = cuda_gl_common->compile_and_link_shader_program_from_files(cuda_gl_common->vertex_shader_file_path.c_str(), cuda_gl_common->frag_shader_file_path.c_str());

	lighting_shader_program = cuda_gl_common->shader_program;

 	texture_load_camera.view_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "view_matrix");
	texture_load_camera.project_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "projection_matrix");
	
	model_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "model_matrix");
	int blue_frag_channel_location = glGetUniformLocation(cuda_gl_common->shader_program, "blue_frag_channel");	

	if (lighting_shader_program <= 0)
	{	
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}

	// glUseProgram(lighting_shader_program);
	// glUniformMatrix4fv(texture_load_camera.view_matrix_location, 1, GL_FALSE, texture_load_camera.view_matrix.m);
	// glUniformMatrix4fv(texture_load_camera.project_matrix_location, 1, GL_FALSE, texture_load_camera.projection_matrix.m);
#pragma endregion

#pragma region Lighting
	/* lights */
	GLuint light_block_location = glGetUniformBlockIndex(lighting_shader_program, "light_source");
	glUniformBlockBinding(lighting_shader_program, light_block_location, 0);


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

	printf("size Light %llu lights %llu lights[] %llu\n", sizeof(Light), sizeof(lights), sizeof(lights[0]));

	// glUseProgram(lighting_shader_program);
	// glUniformMatrix4fv(texture_load_camera.view_matrix_location, 1, GL_FALSE, texture_load_camera.view_matrix.m);
	// glUniformMatrix4fv(texture_load_camera.project_matrix_location, 1, GL_FALSE, texture_load_camera.projection_matrix.m);
#pragma endregion

#pragma region Textures
	/* 2d texture plane geometry */
	GLfloat tex_triangle_points[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	GLfloat tex_triangle_coords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	
	/*
	* May need to rethink how I use VAOs
	* https://stackoverflow.com/questions/59595805/what-is-the-best-way-to-draw-multiple-vao-using-the-same-shader-but-not-having-t
	* https://stackoverflow.com/questions/14249634/opengl-vaos-and-multiple-buffers
	*/

	GLuint vao_texture;
	glGenVertexArrays(1, &vao_texture);
	glBindVertexArray(vao_texture);

	GLuint tex_triangle_points_vbo;
	glGenBuffers(1, &tex_triangle_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tex_triangle_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_triangle_points), tex_triangle_points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tex_triangle_points_vbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	GLuint tex_coords_vbo;
	glGenBuffers(1, &tex_coords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tex_coords_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_triangle_coords), tex_triangle_coords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tex_coords_vbo);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, 0, NULL); // noramlize		
	glEnableVertexAttribArray(3);

	// temp use of model matrix postion 4 to test placing the texure model.	
	model_matrices[4] = translate(identity_mat4(), model_positions_world[4]);

	std::string texture_map_file_path = THIRD_PARTY_ASSETS_DIRECTORY;
	texture_map_file_path.append(CUBE_MAP_FILE_DIRECTORY);

	/* shaders */
	cuda_gl_common->vertex_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->vertex_shader_file_path.append("texture_shader.vert");

	cuda_gl_common->frag_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->frag_shader_file_path.append("texture_shader.frag");
	
	texture_shader_program = cuda_gl_common->compile_and_link_shader_program_from_files(cuda_gl_common->vertex_shader_file_path.c_str(), cuda_gl_common->frag_shader_file_path.c_str());

	if (texture_shader_program <= 0)
	{
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}

	// glUseProgram(texture_shader_program);

	GLuint gl_texture = 0;
	glGenTextures(1, &gl_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_texture);

	bool load_texture_ok = cuda_gl_common->load_texture(std::string(texture_map_file_path).append("posz.jpg").c_str());

	if (load_texture_ok) {
		printf("TEXTURE LOADED OK!\n");
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
#pragma endregion
	
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
		if (texture_load_camera.update_frame(window))
		{
			glfwGetWindowSize(window, &window_width, &window_height);

			/* Check if window was resized */
			texture_load_camera.configure_camera(window_width, window_height);						
			
			/* Clear the drawing sruface */
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);		
			glViewport(0, 0, window_width, window_height);			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
			glUseProgram(lighting_shader_program);
			glUniformMatrix4fv(texture_load_camera.view_matrix_location, 1, GL_FALSE, texture_load_camera.view_matrix.m);
			glUniformMatrix4fv(texture_load_camera.project_matrix_location, 1, GL_FALSE, texture_load_camera.projection_matrix.m);

			// color selected spheres
			for (int i = 0; i < TEXTURE_NUM_OF_SPHERES; i++) {
				
				if (selected_sphere == i) {					
					glUniform1f(blue_frag_channel_location, 1.0f);					
				}
				else {
					glUniform1f(blue_frag_channel_location, 0.0f);
				}

				glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrices[i].m);

				glBindVertexArray(vao_spheres);
				glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
				glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
				glBindBuffer(GL_UNIFORM_BUFFER, lights_vbo);								

				glDrawArrays(GL_TRIANGLES, 0, point_count);
			}			

			if (true) {
				// draw the texture model
				glUseProgram(texture_shader_program);
				glUniformMatrix4fv(texture_load_camera.view_matrix_location, 1, GL_FALSE, texture_load_camera.view_matrix.m);
				glUniformMatrix4fv(texture_load_camera.project_matrix_location, 1, GL_FALSE, texture_load_camera.projection_matrix.m);

				glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, model_matrices[4].m);

				glBindVertexArray(vao_texture);
				glBindBuffer(GL_ARRAY_BUFFER, tex_triangle_points_vbo);
				glBindBuffer(GL_ARRAY_BUFFER, tex_coords_vbo);
				glBindTexture(GL_TEXTURE_2D, gl_texture);

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			
			/* Swap front and back buffers */
			glfwSwapBuffers(window);
		}

		/* Update camera outside FPS loop */
		// TRANSLATIONS			
		texture_load_user_input.process_movements(&texture_load_camera);
				
		// MOUSE PROCESSING
		if (texture_load_user_input.mouse_button_left) {
				
			// vec3 ray_world = process_mouse(window);
			vec3 ray_world = texture_load_user_input.process_mouse(window, &texture_load_camera);

			// sphere check
			int closest_sphere_clicked = -1;
			float closest_intersection = 0.0f;

			for (int i = 0; i < TEXTURE_NUM_OF_SPHERES; i++) {
				float t_distance = 0.0f;

				if (texture_load_ray_intersect(texture_load_camera.camera_position, ray_world, model_positions_world[i], sphere_radius, &t_distance)) {

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
		if (texture_load_user_input.reload_shader_key_pressed) {
			cuda_gl_common->update_shaders(texture_load_camera);
		}

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}