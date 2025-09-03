#pragma once

#include <cuda_gl_common.h>
#include <cuda_gl_user_input.h>
#include <cuda_gl_camera.h>
#include <cuda_gl_lighting.h>

#define FLAT_PLANE_VERTEX_SHADER_FILE "restructured/flat_plane_shader.vert"
#define FLAT_PLANE_FRAGMENT_SHADER_FILE "restructured/flat_plane_shader.frag"

#define PHONG_VERTEX_SHADER_FILE "restructured/phong_shader.vert"
#define PHONG_FRAGMENT_SHADER_FILE "restructured/phong_shader.frag"

#define SPHERE_MESH_FILE "sphere.obj"
#define FLAT_PLANE_MESH_FILE "flat_plane.obj"
#define BUNNY_MESH_FILE "bunny.obj"

#define CUBE_MAP_FILE_DIRECTORY "cube_maps/Yokohama3/"

#define TEXTURE_NUM_OF_SPHERES 4

CUDAGLCamera main_camera;
CUDAGLUserInput user_input;

GLuint restructured_lighting_shader_program = 0;
GLuint restructured_texture_shader_program = 0;

void restructured_init_lights(Light* lights, int number_of_lights) {

	lights[0].light_position_world = vec4(0.0f, 0.0f, 5.0f, 1.0f);
	lights[0].Ls = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	lights[0].Ld = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	lights[0].La = vec4(0.1f, 0.1f, 0.1f, 1.0f);

	lights[0].Ks = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lights[0].Kd = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	lights[0].Ka = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lights[0].specular_exponent = 100.0f;
	
	lights[1].light_position_world = vec4(0.0f, 0.0f, -5.0f, 1.0f);
	lights[1].Ls = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	lights[1].Ld = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	lights[1].La = vec4(0.1f, 0.1f, 0.1f, 1.0f);

	lights[1].Ks = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lights[1].Kd = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	lights[1].Ka = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lights[1].specular_exponent = 100.0f;

	lights[2].light_position_world = vec4(0.0f, 5.0f, 0.0f, 1.0f);
	lights[2].Ls = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	lights[2].Ld = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	lights[2].La = vec4(0.1f, 0.1f, 0.1f, 1.0f);

	lights[2].Ks = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lights[2].Kd = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	lights[2].Ka = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lights[2].specular_exponent = 100.0f;
}

void restructured_init_model_positions(vec3* model_positions, int number_of_models) {
	
	model_positions[0] = vec3(-2.0, 0.0, 0.0);
	model_positions[1] = vec3(2.0, 0.0, 0.0);
	model_positions[2] = vec3(-2.0, -1.0, -2.5);
	model_positions[3] = vec3(2.0, 1.0, -2.5);
	model_positions[4] = vec3(1.0, 1.0, 0.0);
}

void configure_mesh_resources(
	gl_mesh_resources& gl_mesh_resources,
	std::string mesh_file_path,
	std::string mesh_filename	
	) {

	GLfloat* vertex_points = NULL;
	GLfloat* vertex_normals = NULL;
	GLfloat* texture_coordinates = NULL;
	
	mesh_file_path.append(mesh_filename);

	// TODO - replace with assimp: https://learnopengl.com/Model-Loading/Assimp
	load_obj_file(mesh_file_path.c_str(), vertex_points, texture_coordinates, vertex_normals, gl_mesh_resources.mesh_point_count);

	if (NULL != vertex_points) {
			
		// create the VAO
		glGenVertexArrays(1, &(gl_mesh_resources.vertex_array_object_handle));
		glBindVertexArray(gl_mesh_resources.vertex_array_object_handle);

		// vertex points
		glGenBuffers(1, &(gl_mesh_resources.vbo_mesh_points_handle));
		glBindBuffer(GL_ARRAY_BUFFER, gl_mesh_resources.vbo_mesh_points_handle);
		glBufferData(GL_ARRAY_BUFFER, 3 * gl_mesh_resources.mesh_point_count * sizeof(GLfloat), vertex_points, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FLOAT, 0, NULL);
		glEnableVertexAttribArray(0);

		// vertex normals
		glGenBuffers(1, &(gl_mesh_resources.vbo_mesh_normals_handle));
		glBindBuffer(GL_ARRAY_BUFFER, gl_mesh_resources.vbo_mesh_normals_handle);
		glBufferData(GL_ARRAY_BUFFER, 3 * gl_mesh_resources.mesh_point_count * sizeof(GLfloat), vertex_normals, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, 0, NULL);
		glEnableVertexAttribArray(1);

		// texture cordinates - if any
		glGenBuffers(1, &(gl_mesh_resources.vbo_mesh_texture_cordinates_handle));
		glBindBuffer(GL_ARRAY_BUFFER, gl_mesh_resources.vbo_mesh_texture_cordinates_handle);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
		
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, NULL); //normalize
		glEnableVertexAttribArray(2);

		printf("new normals/points/texcoords %i %i %i\n",
			gl_mesh_resources.vbo_mesh_normals_handle,
			gl_mesh_resources.vbo_mesh_points_handle,			
			gl_mesh_resources.vbo_mesh_texture_cordinates_handle
		);
	}
	else {
		fprintf(stderr, "ERROR: could not load meshes.");
		glfwTerminate();
		return;
	}
}

int configure_shader_resources(CUDAGLCommon* cuda_gl_common, gl_shader_resources& gl_shader_resources) {

	std::string vertex_shader_path_and_filename = gl_shader_resources.shader_directory_path + gl_shader_resources.vertex_shader_filename;
	std::string frag_shader_path_and_filename = gl_shader_resources.shader_directory_path + gl_shader_resources.frag_shader_filename;

	gl_shader_resources.shader_program_handle = cuda_gl_common->compile_and_link_shader_program_from_files(vertex_shader_path_and_filename.c_str(), frag_shader_path_and_filename.c_str());

	// TODO handle case where filenames are blank (returns 1)
	if (gl_shader_resources.shader_program_handle <= 0)
	{
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return -1;
	}

	gl_shader_resources.gl_camera_resources.vbo_model_matrix_handle = glGetUniformLocation(gl_shader_resources.shader_program_handle, "model_matrix");
	gl_shader_resources.gl_camera_resources.vbo_projection_matrix_handle = glGetUniformLocation(gl_shader_resources.shader_program_handle, "projection_matrix");
	gl_shader_resources.gl_camera_resources.vbo_view_matrix_handle = glGetUniformLocation(gl_shader_resources.shader_program_handle, "view_matrix");	

	printf("new model/proj/view %i %i %i\n",
		gl_shader_resources.gl_camera_resources.vbo_model_matrix_handle,
		gl_shader_resources.gl_camera_resources.vbo_projection_matrix_handle,
		gl_shader_resources.gl_camera_resources.vbo_view_matrix_handle
	);
	
	return 0;
}

int configure_scene_lighting(gl_lighting_resources& gl_lighting_resources, Light* lights, int number_of_lights, int size_of_lights_in_bytes) {

	glUniformBlockBinding(gl_lighting_resources.associated_shader_program_handle, gl_lighting_resources.vbo_block_lights_location_handle, 0);

	/* https://community.khronos.org/t/sending-an-array-of-structs-to-shader-via-an-uniform-buffer-object/75092 */
	/* https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf */
	glGenBuffers(1, &gl_lighting_resources.vbo_lighting_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, gl_lighting_resources.vbo_lighting_handle);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Light) * number_of_lights, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, gl_lighting_resources.vbo_block_lights_location_handle, gl_lighting_resources.vbo_lighting_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, gl_lighting_resources.vbo_lighting_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, size_of_lights_in_bytes, lights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return 0;
}

/* Restructure goals:
* 1) general code cleanup, more generalized methods.
* 2) transition to using ASSIMP for model importing.
* 3) transition to using deferred shading to enable rendering in phases.
*/
int code_restructured_scene(GLFWwindow* window, CUDAGLCommon* cuda_gl_common) {			

	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	/* user input */
	user_input.init_cuda_gl_user_input(window);

	/* create camera */
	main_camera.init_camera();

	main_camera.configure_camera(
		0.1f,
		100.0f,
		67.0f,
		window_width,
		window_height
	);

	main_camera.place_camera(vec3(0.0f, 0.0f, 10.0f));

#pragma region ASSIMP DEBUG
	/* Assimp import debug */

	std::string asset_filename_and_directory = ASSETS_DIRECTORY;
	asset_filename_and_directory.append(BUNNY_MESH_FILE);
	const aiScene* ai_scene = cuda_gl_common->assimp_scene_from_file(asset_filename_and_directory);

	printf("mesh[0] vertices count %i\n", ai_scene->mMeshes[0]->mNumVertices);

	gl_mesh_resources bunny_mesh_resources;

	cuda_gl_common->extract_mesh_from_assimp_scene(ai_scene, bunny_mesh_resources);

#pragma endregion
			
#pragma region Sphere Geometry and Shaders
	const int number_of_model_positions = 5;

	vec3 model_positions_world[number_of_model_positions];

	restructured_init_model_positions(model_positions_world, number_of_model_positions);

	mat4 model_matrices[TEXTURE_NUM_OF_SPHERES + 1];

	int point_count;

	// DEBUG_20250826 - SPHERE
	gl_shader_resources sphere_shader_resources;
	gl_mesh_resources sphere_mesh_resources;
	gl_lighting_resources sphere_lighting_resources;

	sphere_shader_resources.shader_directory_path = SHADER_DIRECTORY;
	sphere_shader_resources.vertex_shader_filename = PHONG_VERTEX_SHADER_FILE;
	sphere_shader_resources.frag_shader_filename = PHONG_FRAGMENT_SHADER_FILE;

	configure_mesh_resources(sphere_mesh_resources, ASSETS_DIRECTORY, SPHERE_MESH_FILE);
	configure_shader_resources(cuda_gl_common, sphere_shader_resources);

	// DEBUG 20250828 - Texture Map Triangle
	gl_shader_resources flat_plane_shader_resources;
	gl_mesh_resources flat_plane_mesh_resources;
	gl_lighting_resources flat_plane_lighting_resources;

	flat_plane_shader_resources.shader_directory_path = SHADER_DIRECTORY;
	flat_plane_shader_resources.vertex_shader_filename = PHONG_VERTEX_SHADER_FILE; // FLAT_PLANE_VERTEX_SHADER_FILE;
	flat_plane_shader_resources.frag_shader_filename = PHONG_FRAGMENT_SHADER_FILE; // FLAT_PLANE_FRAGMENT_SHADER_FILE;

	configure_mesh_resources(flat_plane_mesh_resources, ASSETS_DIRECTORY, FLAT_PLANE_MESH_FILE);
	configure_shader_resources(cuda_gl_common, flat_plane_shader_resources);
#pragma endregion

#pragma region Texture application

	/* TODO: See defered shading goal */

#pragma endregion
		
#pragma region Lighting
	const int number_of_lights = 3;

	Light lights[number_of_lights];

	restructured_init_lights(lights, 3);

	const float sphere_radius = 1.0f;	

	int size_of_lights_in_bytes = sizeof(lights);

	sphere_lighting_resources.associated_shader_program_handle = sphere_shader_resources.shader_program_handle;
	sphere_lighting_resources.vbo_block_lights_location_handle = glGetUniformBlockIndex(sphere_lighting_resources.associated_shader_program_handle, "light_source");

	// Sphere
	configure_scene_lighting(
		sphere_lighting_resources,
		lights,
		number_of_lights,
		size_of_lights_in_bytes
	);

	flat_plane_lighting_resources.associated_shader_program_handle = flat_plane_shader_resources.shader_program_handle;
	flat_plane_lighting_resources.vbo_block_lights_location_handle = glGetUniformBlockIndex(flat_plane_lighting_resources.associated_shader_program_handle, "light_source");

	// Flat Plane
	configure_scene_lighting(
		flat_plane_lighting_resources,
		lights,
		number_of_lights,
		size_of_lights_in_bytes
	);
#pragma endregion

	cuda_gl_common->set_opengl_flags();

	/* 0 swap immediate 1 sync to monitor */
	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window))
	{					
		if (main_camera.update_frame(window))
		{
			glfwGetWindowSize(window, &window_width, &window_height);

			/* Check if window was resized */
			main_camera.configure_camera(window_width, window_height);						
			
			/* Clear the drawing sruface */
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);		
			glViewport(0, 0, window_width, window_height);			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// draw spheres - work in progress
			if (true) {
				
				glUseProgram(sphere_shader_resources.shader_program_handle);
				glUniformMatrix4fv(sphere_shader_resources.gl_camera_resources.vbo_view_matrix_handle , 1, GL_FALSE, main_camera.view_matrix.m);
				glUniformMatrix4fv(sphere_shader_resources.gl_camera_resources.vbo_projection_matrix_handle, 1, GL_FALSE, main_camera.projection_matrix.m);
				
				for (int i = 0; i < TEXTURE_NUM_OF_SPHERES; i++) {

					model_matrices[i] = translate(identity_mat4(), model_positions_world[i]);

					glUniformMatrix4fv(sphere_shader_resources.gl_camera_resources.vbo_model_matrix_handle, 1, GL_FALSE, model_matrices[i].m);

					glBindVertexArray(sphere_mesh_resources.vertex_array_object_handle);
					glBindBuffer(GL_ARRAY_BUFFER, sphere_mesh_resources.vbo_mesh_points_handle);
					glBindBuffer(GL_ARRAY_BUFFER, sphere_mesh_resources.vbo_mesh_normals_handle);
					glBindBuffer(GL_UNIFORM_BUFFER, sphere_lighting_resources.vbo_lighting_handle);

					glDrawArrays(GL_TRIANGLES, 0, sphere_mesh_resources.mesh_point_count);
				}
			}

			// draw the texture model - WIP
			glUseProgram(flat_plane_shader_resources.shader_program_handle);
			glUniformMatrix4fv(flat_plane_shader_resources.gl_camera_resources.vbo_view_matrix_handle, 1, GL_FALSE, main_camera.view_matrix.m);
			glUniformMatrix4fv(flat_plane_shader_resources.gl_camera_resources.vbo_projection_matrix_handle, 1, GL_FALSE, main_camera.projection_matrix.m);

			model_matrices[TEXTURE_NUM_OF_SPHERES] = translate(identity_mat4(), model_positions_world[TEXTURE_NUM_OF_SPHERES]);

			glUniformMatrix4fv(flat_plane_shader_resources.gl_camera_resources.vbo_model_matrix_handle, 1, GL_FALSE, model_matrices[TEXTURE_NUM_OF_SPHERES].m);

			glBindVertexArray(flat_plane_mesh_resources.vertex_array_object_handle);
			glBindBuffer(GL_ARRAY_BUFFER, flat_plane_mesh_resources.vbo_mesh_points_handle);
			glBindBuffer(GL_ARRAY_BUFFER, flat_plane_mesh_resources.vbo_mesh_normals_handle);
			glBindBuffer(GL_UNIFORM_BUFFER, flat_plane_lighting_resources.vbo_lighting_handle);

			glDrawArrays(GL_TRIANGLES, 0, flat_plane_mesh_resources.mesh_point_count);			
			
			/* Swap front and back buffers */
			glfwSwapBuffers(window);
		}

		/* Update camera outside FPS loop */
		// TRANSLATIONS			
		user_input.process_movements(&main_camera);
				
		// MOUSE PROCESSING
		if (user_input.mouse_button_left) {
			// do nothing			
		}

		// PROCESS SHADER RELOAD
		if (user_input.reload_shader_key_pressed) {
			cuda_gl_common->update_shaders(main_camera);
		}

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}