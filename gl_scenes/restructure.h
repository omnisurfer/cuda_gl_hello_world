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

void init_light_positions(Light* lights, int number_of_lights) {

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

void init_model_positions(vec3* model_positions, int number_of_models) {
	
	model_positions[0] = vec3(-2.0, 0.0, 0.0);
	model_positions[1] = vec3(2.0, 0.0, 0.0);
	model_positions[2] = vec3(-2.0, -1.0, -2.5);
	model_positions[3] = vec3(2.0, 1.0, -2.5);
	model_positions[4] = vec3(1.0, 1.0, 0.0);
}

int configure_and_compile_shader_resources(CUDAGLCommon* cuda_gl_common, gl_shader_resources& gl_shader_resources) {

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
	
	return 0;
}

int bind_camera_matrices_to_shader_resources(gl_shader_resources& gl_shader_resources) {

	gl_shader_resources.gl_camera_resources.vbo_model_matrix_handle = glGetUniformLocation(gl_shader_resources.shader_program_handle, "model_matrix");
	gl_shader_resources.gl_camera_resources.vbo_projection_matrix_handle = glGetUniformLocation(gl_shader_resources.shader_program_handle, "projection_matrix");
	gl_shader_resources.gl_camera_resources.vbo_view_matrix_handle = glGetUniformLocation(gl_shader_resources.shader_program_handle, "view_matrix");

	printf("bind model/proj/view %i %i %i\n",
		gl_shader_resources.gl_camera_resources.vbo_model_matrix_handle,
		gl_shader_resources.gl_camera_resources.vbo_projection_matrix_handle,
		gl_shader_resources.gl_camera_resources.vbo_view_matrix_handle
	);

	return 0;
}

int configure_texture_resources(CUDAGLCommon* cuda_gl_common, GLuint& gl_texture_handle) {

	std::string texture_map_file_path = THIRD_PARTY_ASSETS_DIRECTORY;
	texture_map_file_path.append(CUBE_MAP_FILE_DIRECTORY);

	glGenTextures(1, &gl_texture_handle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_texture_handle);

	bool load_texture_ok = cuda_gl_common->load_texture_into_device_memory(std::string(texture_map_file_path).append("posz.png").c_str());

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
* X 1) general code cleanup, more generalized methods.
* X 2) transition to using ASSIMP for model importing.
* 3) transition to using deferred shading to enable rendering in phases.
*	a) textures?
*	b) lighting
*/
/* Example code:
* https://github.com/capnramses/antons_opengl_tutorials_book/blob/master/37_deferred_shading/main.cpp
* https://github.com/JoeyDeVries/LearnOpenGL/tree/master/src/5.advanced_lighting/8.1.deferred_shading
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

	std::string asset_filename_and_directory = ASSETS_DIRECTORY;
			
#pragma region Geometry
	const int number_of_model_positions = 5;
	vec3 model_positions_world[number_of_model_positions];

	init_model_positions(model_positions_world, number_of_model_positions);

	mat4 model_matrices[TEXTURE_NUM_OF_SPHERES + 1];
					
	const aiScene* ai_scene;

	// Sphere Mesh
	gl_mesh_resources sphere_mesh_resources;
	ai_scene = cuda_gl_common->assimp_scene_from_file(asset_filename_and_directory + SPHERE_MESH_FILE);
	cuda_gl_common->assimp_extract_and_load_mesh_from_scene(ai_scene, sphere_mesh_resources);	

	// Bunny Mesh
	gl_mesh_resources bunny_mesh_resources;
	ai_scene = cuda_gl_common->assimp_scene_from_file(asset_filename_and_directory + BUNNY_MESH_FILE);		
	cuda_gl_common->assimp_extract_and_load_mesh_from_scene(ai_scene, bunny_mesh_resources);	
	
	// Flat Plane Mesh - Textur Map TBD
	gl_mesh_resources flat_plane_mesh_resources;
	ai_scene = cuda_gl_common->assimp_scene_from_file(asset_filename_and_directory + FLAT_PLANE_MESH_FILE);	
	cuda_gl_common->assimp_extract_and_load_mesh_from_scene(ai_scene, flat_plane_mesh_resources);	
#pragma endregion

#pragma region Texture application
	gl_shader_resources texture_shader_resources;

	GLuint gl_texture_handle = 0;

	/* TODO: See deferred shading goal */	
	texture_shader_resources.shader_directory_path = SHADER_DIRECTORY;
	texture_shader_resources.vertex_shader_filename = FLAT_PLANE_VERTEX_SHADER_FILE;
	texture_shader_resources.frag_shader_filename = FLAT_PLANE_FRAGMENT_SHADER_FILE;

	configure_and_compile_shader_resources(cuda_gl_common, texture_shader_resources);
	bind_camera_matrices_to_shader_resources(texture_shader_resources);
	configure_texture_resources(cuda_gl_common, gl_texture_handle);
#pragma endregion
		
#pragma region Phong Lighting
	gl_shader_resources phong_lighting_shader_resources;	
	gl_lighting_resources phong_lighting_handle_resources;

	phong_lighting_shader_resources.shader_directory_path = SHADER_DIRECTORY;
	phong_lighting_shader_resources.vertex_shader_filename = PHONG_VERTEX_SHADER_FILE;
	phong_lighting_shader_resources.frag_shader_filename = PHONG_FRAGMENT_SHADER_FILE;

	configure_and_compile_shader_resources(cuda_gl_common, phong_lighting_shader_resources);
	bind_camera_matrices_to_shader_resources(phong_lighting_shader_resources);

	const int number_of_lights = 3;

	Light lights[number_of_lights];

	init_light_positions(lights, number_of_lights);

	int size_of_lights_in_bytes = sizeof(lights);

	phong_lighting_handle_resources.associated_shader_program_handle = phong_lighting_shader_resources.shader_program_handle;
	phong_lighting_handle_resources.vbo_block_lights_location_handle = glGetUniformBlockIndex(phong_lighting_handle_resources.associated_shader_program_handle, "light_source");
	
	configure_scene_lighting(
		phong_lighting_handle_resources,
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

			// draw sphere meshes
			if (true) {
				
				glUseProgram(phong_lighting_shader_resources.shader_program_handle);
				glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_view_matrix_handle , 1, GL_FALSE, main_camera.view_matrix.m);
				glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_projection_matrix_handle, 1, GL_FALSE, main_camera.projection_matrix.m);
				
				for (int i = 0; i < TEXTURE_NUM_OF_SPHERES; i++) {

					model_matrices[i] = translate(identity_mat4(), model_positions_world[i]);

					glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_model_matrix_handle, 1, GL_FALSE, model_matrices[i].m);

					glBindVertexArray(sphere_mesh_resources.vertex_array_object_handle);
					glBindBuffer(GL_ARRAY_BUFFER, sphere_mesh_resources.vbo_mesh_points_handle);
					glBindBuffer(GL_ARRAY_BUFFER, sphere_mesh_resources.vbo_mesh_normals_handle);
					glBindBuffer(GL_UNIFORM_BUFFER, phong_lighting_handle_resources.vbo_lighting_handle);

					glDrawArrays(GL_TRIANGLES, 0, sphere_mesh_resources.mesh_point_count);
				}
			}

			// draw bunny mesh
			if (true) {
				// glUseProgram(phong_lighting_shader_resources.shader_program_handle);
				// glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_view_matrix_handle, 1, GL_FALSE, main_camera.view_matrix.m);
				// glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_projection_matrix_handle, 1, GL_FALSE, main_camera.projection_matrix.m);

				model_matrices[TEXTURE_NUM_OF_SPHERES] = translate(identity_mat4(), model_positions_world[TEXTURE_NUM_OF_SPHERES]);

				glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_model_matrix_handle, 1, GL_FALSE, model_matrices[TEXTURE_NUM_OF_SPHERES].m);

				glBindVertexArray(bunny_mesh_resources.vertex_array_object_handle);
				glBindBuffer(GL_ARRAY_BUFFER, bunny_mesh_resources.vbo_mesh_points_handle);
				glBindBuffer(GL_ARRAY_BUFFER, bunny_mesh_resources.vbo_mesh_normals_handle);
				glBindBuffer(GL_UNIFORM_BUFFER, phong_lighting_handle_resources.vbo_lighting_handle);

				glDrawArrays(GL_TRIANGLES, 0, bunny_mesh_resources.mesh_point_count);
			}

			// draw the texture model - WIP
			if (true) {

				// WIP - use the texutre fragment shader
				if (true) {
					glUseProgram(texture_shader_resources.shader_program_handle);
					glUniformMatrix4fv(texture_shader_resources.gl_camera_resources.vbo_view_matrix_handle, 1, GL_FALSE, main_camera.view_matrix.m);
					glUniformMatrix4fv(texture_shader_resources.gl_camera_resources.vbo_projection_matrix_handle, 1, GL_FALSE, main_camera.projection_matrix.m);

					model_matrices[TEXTURE_NUM_OF_SPHERES] = translate(identity_mat4(), model_positions_world[TEXTURE_NUM_OF_SPHERES]);

					glUniformMatrix4fv(texture_shader_resources.gl_camera_resources.vbo_model_matrix_handle, 1, GL_FALSE, model_matrices[TEXTURE_NUM_OF_SPHERES].m);

					glBindVertexArray(flat_plane_mesh_resources.vertex_array_object_handle);
					glBindBuffer(GL_ARRAY_BUFFER, flat_plane_mesh_resources.vbo_mesh_points_handle);
					glBindBuffer(GL_ARRAY_BUFFER, flat_plane_mesh_resources.vbo_mesh_normals_handle);
					glBindBuffer(GL_ARRAY_BUFFER, flat_plane_mesh_resources.vbo_mesh_texture_cordinates_handle);
					glBindTexture(GL_TEXTURE_2D, gl_texture_handle);

					glDrawArrays(GL_TRIANGLES, 0, flat_plane_mesh_resources.mesh_point_count);
				}

				// then add lighting
				if (false) {
					glUseProgram(phong_lighting_shader_resources.shader_program_handle);
					glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_view_matrix_handle, 1, GL_FALSE, main_camera.view_matrix.m);
					glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_projection_matrix_handle, 1, GL_FALSE, main_camera.projection_matrix.m);

					model_matrices[TEXTURE_NUM_OF_SPHERES] = translate(identity_mat4(), model_positions_world[TEXTURE_NUM_OF_SPHERES]);

					glUniformMatrix4fv(phong_lighting_shader_resources.gl_camera_resources.vbo_model_matrix_handle, 1, GL_FALSE, model_matrices[TEXTURE_NUM_OF_SPHERES].m);

					glBindVertexArray(flat_plane_mesh_resources.vertex_array_object_handle);
					glBindBuffer(GL_ARRAY_BUFFER, flat_plane_mesh_resources.vbo_mesh_points_handle);
					glBindBuffer(GL_ARRAY_BUFFER, flat_plane_mesh_resources.vbo_mesh_normals_handle);
					glBindBuffer(GL_UNIFORM_BUFFER, phong_lighting_handle_resources.vbo_lighting_handle);

					glDrawArrays(GL_TRIANGLES, 0, flat_plane_mesh_resources.mesh_point_count);
				}				
			}
			
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