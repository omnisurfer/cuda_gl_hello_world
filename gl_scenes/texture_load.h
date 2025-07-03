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
bool texture_load_ray_intersect(
	vec3 ray_origin_world, 
	vec3 ray_direction_world, 
	vec3 sphere_center_world, 
	float sphere_radius, 
	float* intersection_distance
) {	
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

void init_lights(Light* lights, int number_of_lights) {

	lights[0].light_position_world = vec4(0.0, 0.0, 5.0, 1.0);
	lights[0].Ls = vec4(0.2, 0.2, 0.2, 1.0);
	lights[0].Ld = vec4(0.7, 0.7, 0.7, 1.0);
	lights[0].La = vec4(0.1, 0.1, 0.1, 1.0);

	lights[0].Ks = vec4(1.0, 1.0, 1.0, 1.0);
	lights[0].Kd = vec4(1.0, 0.0, 0.0, 1.0);
	lights[0].Ka = vec4(1.0, 1.0, 1.0, 1.0);
	lights[0].specular_exponent = 100.0;
	
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
}

void init_model_positions(vec3* model_positions, int number_of_models) {
	
	model_positions[0] = vec3(-2.0, 0.0, 0.0);
	model_positions[1] = vec3(2.0, 0.0, 0.0);
	model_positions[2] = vec3(-2.0, -1.0, -2.5);
	model_positions[3] = vec3(2.0, 1.0, -2.5);
	model_positions[4] = vec3(1.0, 1.0, 0.0);
}

void configure_resources_spheres(
	GLuint& vao_spheres,
	GLuint& points_vbo,
	GLuint& normals_vbo,
	mat4* model_matrices,
	vec3* model_positions_world,
	int& point_count
) {

	GLfloat* vertex_points = NULL;
	GLfloat* vertex_normals = NULL;
	GLfloat* texture_coordinates = NULL;	

	std::string mesh_file_path = ASSETS_DIRECTORY;
	mesh_file_path.append(TEXTURE_MESH_FILE);

	load_obj_file(mesh_file_path.c_str(), vertex_points, texture_coordinates, vertex_normals, point_count);

	glGenVertexArrays(1, &vao_spheres);
	glBindVertexArray(vao_spheres);

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
		return;
	}

	for (int i = 0; i < TEXTURE_NUM_OF_SPHERES; i++) {
		model_matrices[i] = translate(identity_mat4(), model_positions_world[i]);
	}
}

int configure_shaders_spheres(CUDAGLCommon* cuda_gl_common, GLuint &view_matrix_vbo, GLuint &projection_matrix_vbo, GLuint &model_matrix_vbo) {

	cuda_gl_common->vertex_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->vertex_shader_file_path.append(TEXTURE_VERTEX_SHADER_FILE);

	cuda_gl_common->frag_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->frag_shader_file_path.append(TEXTURE_FRAGMENT_SHADER_FILE);

	cuda_gl_common->shader_program = cuda_gl_common->compile_and_link_shader_program_from_files(cuda_gl_common->vertex_shader_file_path.c_str(), cuda_gl_common->frag_shader_file_path.c_str());

	lighting_shader_program = cuda_gl_common->shader_program;

	texture_load_camera.view_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "view_matrix");
	texture_load_camera.project_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "projection_matrix");

	model_matrix_vbo = glGetUniformLocation(cuda_gl_common->shader_program, "model_matrix");	

	if (lighting_shader_program <= 0)
	{
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return -1;
	}
}

int configure_scene_lighting(GLuint &lights_vbo, GLuint &light_block_location, Light* lights, int size_of_lights_in_bytes, int number_of_lights) {

	glUniformBlockBinding(lighting_shader_program, light_block_location, 0);

	/* https://community.khronos.org/t/sending-an-array-of-structs-to-shader-via-an-uniform-buffer-object/75092 */
	/* https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf */
	glGenBuffers(1, &lights_vbo);
	glBindBuffer(GL_UNIFORM_BUFFER, lights_vbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * number_of_lights, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, light_block_location, lights_vbo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, lights_vbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, size_of_lights_in_bytes, lights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return 0;
}

void init_textures_points(GLfloat *texture_triangle_points, int number_of_triangles) {
	
	float _texture_triangle_points[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	for (int i = 0; i < number_of_triangles * 9; i++) {
		texture_triangle_points[i] = _texture_triangle_points[i];
	}	
}

void configure_resources_texture(
	GLuint& vao_texture,
	GLuint& tex_triangle_points_vbo,
	GLuint& tex_coords_vbo,
	mat4* model_matrices,
	vec3* model_positions_world,
	GLfloat* tex_triangle_points,
	int size_of_tex_triangle_points,
	GLfloat* tex_triangle_coords,
	int size_of_tex_triangle_coords
) {

	glGenVertexArrays(1, &vao_texture);
	glBindVertexArray(vao_texture);

	glGenBuffers(1, &tex_triangle_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tex_triangle_points_vbo);
	glBufferData(GL_ARRAY_BUFFER, size_of_tex_triangle_points, tex_triangle_points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tex_triangle_points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &tex_coords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tex_coords_vbo);
	glBufferData(GL_ARRAY_BUFFER, size_of_tex_triangle_coords, tex_triangle_coords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tex_coords_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 0, NULL); // noramlize
	glEnableVertexAttribArray(1);

	// temp use of model matrix postion 4 to test placing the texure model.	
	model_matrices[4] = translate(identity_mat4(), model_positions_world[4]);
}

int configure_shaders_texture(
	CUDAGLCommon* cuda_gl_common, 
	GLuint& gl_texture, 
	GLuint& view_matrix_vbo, 
	GLuint& projection_matrix_vbo, 
	GLuint& model_matrix_vbo
) {

	std::string texture_map_file_path = THIRD_PARTY_ASSETS_DIRECTORY;
	texture_map_file_path.append(CUBE_MAP_FILE_DIRECTORY);

	/* shaders */
	cuda_gl_common->vertex_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->vertex_shader_file_path.append("texture_shader.vert");

	cuda_gl_common->frag_shader_file_path = SHADER_DIRECTORY;
	cuda_gl_common->frag_shader_file_path.append("texture_shader.frag");

	cuda_gl_common->shader_program = cuda_gl_common->compile_and_link_shader_program_from_files(cuda_gl_common->vertex_shader_file_path.c_str(), cuda_gl_common->frag_shader_file_path.c_str());
	
	texture_shader_program = cuda_gl_common->shader_program;

	/*
	texture_load_camera.view_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "view_matrix");
	texture_load_camera.project_matrix_location = glGetUniformLocation(cuda_gl_common->shader_program, "projection_matrix");

	model_matrix_vbo = glGetUniformLocation(cuda_gl_common->shader_program, "model_matrix");
	*/

	if (texture_shader_program <= 0)
	{
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}
	
	if (true) {
		glGenTextures(1, &gl_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		bool load_texture_ok = cuda_gl_common->load_texture(std::string(texture_map_file_path).append("posz.jpg").c_str());

		if (!load_texture_ok) {
			printf("Texture failed to load properly!\n");
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		GLfloat max_ansio = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_ansio);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_ansio);
	}

	return 0;
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
		
#pragma region Sphere Geometry and Shaders
	const int number_of_model_positions = 5;

	vec3 model_positions_world[number_of_model_positions];

	init_model_positions(model_positions_world, number_of_model_positions);
	
	mat4 model_matrices[TEXTURE_NUM_OF_SPHERES + 1];

	GLuint vao_spheres;
	GLuint points_vbo;
	GLuint normals_vbo;

	GLuint view_matrix_vbo;
	GLuint projection_matrix_vbo;
	GLuint model_matrix_vbo;

	int point_count;

	configure_resources_spheres(vao_spheres, points_vbo, normals_vbo, model_matrices, model_positions_world, point_count);

	configure_shaders_spheres(cuda_gl_common, view_matrix_vbo, projection_matrix_vbo, model_matrix_vbo);
#pragma endregion

#pragma region Lighting	

	const int number_of_lights = 3;

	Light lights[number_of_lights];

	init_lights(lights, 3);

	const float sphere_radius = 1.0f;

	GLuint lights_vbo;
	GLuint light_block_location = glGetUniformBlockIndex(lighting_shader_program, "light_source");

	int size_of_lights_in_bytes = sizeof(lights);
		
	configure_scene_lighting(
		lights_vbo,
		light_block_location,
		lights,
		size_of_lights_in_bytes,
		number_of_lights
	);	
#pragma endregion

#pragma region Textures
	/* 2d texture plane geometry */
	const int number_of_triangles = 2;
	const int points_per_triangle = 9;
	const int number_of_triangle_points = number_of_triangles * points_per_triangle;

	GLfloat tex_triangle_points[number_of_triangle_points];

	init_textures_points(tex_triangle_points, number_of_triangles);

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
	GLuint tex_triangle_points_vbo;
	GLuint tex_coords_vbo;
	
	int size_of_triangle_points = sizeof(tex_triangle_points);
	int size_of_tex_triangle_coords = sizeof(tex_triangle_coords);

	configure_resources_texture(
		vao_texture,
		tex_triangle_points_vbo,
		tex_coords_vbo,
		model_matrices,
		model_positions_world,
		tex_triangle_points,
		size_of_triangle_points,
		tex_triangle_coords,
		size_of_tex_triangle_coords
	);
	
	GLuint gl_texture = 0;

	configure_shaders_texture(cuda_gl_common, gl_texture, view_matrix_vbo, projection_matrix_vbo, model_matrix_vbo);
#pragma endregion
		
	cuda_gl_common->set_opengl_flags();

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

			// draw spheres
			if (true) {
				glUseProgram(lighting_shader_program);
				glUniformMatrix4fv(texture_load_camera.view_matrix_location, 1, GL_FALSE, texture_load_camera.view_matrix.m);
				glUniformMatrix4fv(texture_load_camera.project_matrix_location, 1, GL_FALSE, texture_load_camera.projection_matrix.m);

				// color selected spheres
				for (int i = 0; i < TEXTURE_NUM_OF_SPHERES; i++) {

					glUniformMatrix4fv(model_matrix_vbo, 1, GL_FALSE, model_matrices[i].m);
					
					glBindVertexArray(vao_spheres);
					glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
					glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
					glBindBuffer(GL_UNIFORM_BUFFER, lights_vbo);

					glDrawArrays(GL_TRIANGLES, 0, point_count);
				}
			}

			// draw the texture model
			if (true) {
				glUseProgram(texture_shader_program);
				glUniformMatrix4fv(texture_load_camera.view_matrix_location, 1, GL_FALSE, texture_load_camera.view_matrix.m);
				glUniformMatrix4fv(texture_load_camera.project_matrix_location, 1, GL_FALSE, texture_load_camera.projection_matrix.m);
				
				// This causes this error:
				// API ERROR HIGH message GL_INVALID_OPERATION error generated. Uniform must be a matrix type in call to UniformMatrix*. userParam -1				
				// glUniformMatrix4fv(model_matrix_vbo, 1, GL_FALSE, model_matrices[4].m);

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
			// do nothing			
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