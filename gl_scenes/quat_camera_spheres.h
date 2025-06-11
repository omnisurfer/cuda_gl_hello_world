#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <fstream>

#include <cuda_gl_setup_utils.h>
// #include <cuda_gl_scene_utils.h>

#include <cuda_gl_camera.h>

#define SPHERE_VERTEX_SHADER_FILE "quat_camera_shader.vert"
#define SPHERE_FRAGMENT_SHADER_FILE "quat_camera_shader.frag"

#define MESH_FILE "3d_objects/sphere.obj"

#define NUM_OF_SPHERS 4

scene_key_callback_ptr scene_key_callback_function_ptr = nullptr;
scene_mouse_button_callback_ptr scene_mouse_button_callback_function_ptr = nullptr;

GLuint shader_program;

// mat4 view_matrix;
// mat4 projection_matrix;

int model_matrix_location = 0;
int view_matrix_location = 0;
int project_matrix_location = 0;

std::string vertex_shader_file_path;
std::string frag_shader_file_path;

double delta_time = 0.0f;

vec3 camera_position;

// TODO turn into bitfield?
bool move_forward = false;
bool move_backward = false;

bool move_left = false;
bool move_right = false;

bool move_up = false;
bool move_down = false;

bool roll_right = false;
bool roll_left = false;

bool pitch_up = false;
bool pitch_down = false;

bool yaw_right = false;
bool yaw_left = false;

bool mouse_button_left = false;
bool mouse_button_middle = false;
bool mouse_button_right = false;

BasicCUDAGLCamera camera;

vec3 get_ray_from_mouse_coords(GLFWwindow* window, float mouse_x, float mouse_y) {

	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	// screen space (viewport coordinates)
	float x = (2.0f * mouse_x) / (float)window_width - 1.0f;
	float y = 1.0f - (2.0f * mouse_y) / (float)window_height;
	float z = 1.0f;

	vec3 ray_normalized_device_space = vec3(x, y, z);
	
	if (false) {
		printf("dev space %f %f %f\n",
			ray_normalized_device_space.v[0],
			ray_normalized_device_space.v[1],
			ray_normalized_device_space.v[2]
		);
	}

	// clip space
	vec4 ray_clip_space = vec4(ray_normalized_device_space.v[0], ray_normalized_device_space.v[1], -1.0, 1.0);
	
	if (false) {
		printf("clip space %f %f %f %f\n",
			ray_clip_space.v[0],
			ray_clip_space.v[1],
			ray_clip_space.v[2],
			ray_clip_space.v[3]
		);
	}

	// eye space
	vec4 ray_eye_space = inverse(camera.projection_matrix) * ray_clip_space;

	if (false) {
		printf("eye space pre %f %f %f %f\n",
			ray_eye_space.v[0],
			ray_eye_space.v[1],
			ray_eye_space.v[2],
			ray_eye_space.v[3]
		);
	}

	ray_eye_space = vec4(ray_eye_space.v[0], ray_eye_space.v[1], -1.0, 0.0);

	if (false) {
		printf("eye space post %f %f %f %f\n",
			ray_eye_space.v[0],
			ray_eye_space.v[1],
			ray_eye_space.v[2],
			ray_eye_space.v[3]
		);
	}

	// world space
	vec3 ray_world_space = vec3(inverse(camera.view_matrix) * ray_eye_space);
	
	if (false) {
		printf("world space raw %f %f %f\n",
			ray_world_space.v[0],
			ray_world_space.v[1],
			ray_world_space.v[2]
		);
	}

	ray_world_space = normalise(ray_world_space);

	if (false) {
		printf("world space norm %f %f %f\n",
			ray_world_space.v[0],
			ray_world_space.v[1],
			ray_world_space.v[2]
		);
	}

	return ray_world_space;
}

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

void scene_key_callback_function(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// printf("HELLO FROM scene_key_callback_function %i %i %i %i\n", key, scancode, action, mods);

	// forward, back, left, right, up, down
	if (glfwGetKey(window, GLFW_KEY_W)) {		
		move_forward = true;
	}
	else {
		move_forward = false;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		move_left = true;		
	}
	else {
		move_left = false;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		move_backward = true;
	}
	else {
		move_backward = false;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		move_right = true;
	}
	else {
		move_right = false;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		move_up = true;
	}
	else {
		move_up = false;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		move_down = true;
	}
	else {
		move_down = false;
	}

	// roll, pitch, yaw
	if (glfwGetKey(window, GLFW_KEY_Z)) {
		roll_left = true;
	}
	else {
		roll_left = false;
	}
	if (glfwGetKey(window, GLFW_KEY_X)) {
		roll_right = true;
	}
	else {
		roll_right = false;
	}	
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		pitch_up = true;
	}
	else {
		pitch_up = false;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		pitch_down = true;
	}	
	else {
		pitch_down = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		yaw_left = true;
	}
	else {
		yaw_left = false;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		yaw_right = true;
	}
	else {
		yaw_right = false;
	}

	// reload shader
	if (key == GLFW_KEY_R && action == 0) {
		
		if (true) {
			printf("Recompiling shaders %s\n", frag_shader_file_path.c_str());

			glDeleteProgram(shader_program);

			shader_program = compile_and_link_shader_program_from_files(
				vertex_shader_file_path.c_str(),
				frag_shader_file_path.c_str()
			);

			if (shader_program > 0)
			{				
				glUseProgram(shader_program);
				
				if (shader_program > 0)
				{
					glUseProgram(shader_program);
					glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, camera.view_matrix.m);
					glUniformMatrix4fv(project_matrix_location, 1, GL_FALSE, camera.projection_matrix.m);
				}
			}
		}
	}
}

void scene_mouse_button_callback_function(GLFWwindow* window, int button, int action, int mods) {

	if (action == GLFW_PRESS) {

		printf("press ");

		if (button == GLFW_MOUSE_BUTTON_1) {
			printf("button 1\n");
			mouse_button_left = true;
		}		

		if (button == GLFW_MOUSE_BUTTON_2) {
			printf("button 2\n");
			mouse_button_right = true;
		}		

		if (button == GLFW_MOUSE_BUTTON_3) {
			printf("button 3\n");
			mouse_button_middle = true;
		}		
	}
	else if (action == GLFW_RELEASE) {

		printf("relase ");

		if (button == GLFW_MOUSE_BUTTON_1) {
			printf("button 1\n");
			mouse_button_left = false;
		}		

		if (button == GLFW_MOUSE_BUTTON_2) {
			printf("button 2\n");
			mouse_button_right = false;
		}		

		if (button == GLFW_MOUSE_BUTTON_3) {
			printf("button 3\n");
			mouse_button_middle = false;
		}		
	}
}

void process_movements(float move_delta) {

	bool camera_moved = false;

	if (true) {
		if (move_left) {
			camera_moved = true;
			camera.move_camera_x(-move_delta);
		}
		if (move_right) {
			camera_moved = true;
			camera.move_camera_x(move_delta);
		}
		if (move_up) {
			camera_moved = true;
			camera.move_camera_y(-move_delta);
		}
		if (move_down) {
			camera_moved = true;
			camera.move_camera_y(move_delta);
		}
		if (move_forward) {
			camera_moved = true;
			camera.move_camera_z(-move_delta);
		}
		if (move_backward) {
			camera_moved = true;
			camera.move_camera_z(move_delta);
		}

		// ROTATIONS		
		float rotation_delta = camera.camera_heading_speed * (float)delta_time;
		if (roll_left) {
			camera_moved = true;
			camera.roll_camera(rotation_delta);
		}
		if (roll_right) {
			camera_moved = true;
			camera.roll_camera(-rotation_delta);
		}
		if (pitch_up) {
			camera_moved = true;
			camera.pitch_camera(rotation_delta);
		}
		if (pitch_down) {
			camera_moved = true;
			camera.pitch_camera(-rotation_delta);
		}
		if (yaw_left) {
			camera_moved = true;
			camera.yaw_camera(rotation_delta);
		}
		if (yaw_right) {
			camera_moved = true;
			camera.yaw_camera(-rotation_delta);
		}		
	}

	if (camera_moved) {
		camera.move_camera();
		glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, camera.view_matrix.m);
	}	
}

vec3 process_mouse(GLFWwindow* window) {
		
		double x_position, y_position;

		glfwGetCursorPos(window, &x_position, &y_position);

		// printf("x_pos %f y_pos %f\n", x_position, y_position);

		return get_ray_from_mouse_coords(window, (float)x_position, (float)y_position);
}

/* From 06_vcam_with_quaternion */
int draw_quat_cam_spheres(GLFWwindow* window) {
	
	scene_key_callback_function_ptr = scene_key_callback_function;
	
	if (scene_key_callback_function_ptr != nullptr) {
		set_scene_key_callback_function(scene_key_callback_function_ptr);
	}

	scene_mouse_button_callback_function_ptr = scene_mouse_button_callback_function;

	if (scene_mouse_button_callback_function_ptr != nullptr) {
		set_scene_mouse_button_callback_function(scene_mouse_button_callback_function_ptr);
	}

	vec3 sphere_positions_world[] = {
		vec3(-2.0, 0.0, 0.0),
		vec3(2.0, 0.0, 0.0),
		vec3(-2.0, 0.0, -2.0),
		vec3(1.5, 1.0, -1.0),
	};

	const float sphere_radius = 1.0f;
	
	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	/* create camera */
	camera.configure_camera(
		0.1f,
		100.0f,
		67.0f,
		window_width,
		window_height
	);
	
	camera.init_camera();

	camera.camera_position = vec3(0.0f, 0.0f, 5.0f);

	camera.place_camera();

	/* create geometry */
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

	/* Create shaders */
	vertex_shader_file_path = SHADER_DIRECTORY;
	vertex_shader_file_path.append(SPHERE_VERTEX_SHADER_FILE);

	frag_shader_file_path = SHADER_DIRECTORY;
	frag_shader_file_path.append(SPHERE_FRAGMENT_SHADER_FILE);

	shader_program = compile_and_link_shader_program_from_files(vertex_shader_file_path.c_str(), frag_shader_file_path.c_str());
	model_matrix_location = glGetUniformLocation(shader_program, "model_matrix");
 	view_matrix_location = glGetUniformLocation(shader_program, "view_matrix");
	project_matrix_location = glGetUniformLocation(shader_program, "projection_matrix");
	int blue_frag_channel_location = glGetUniformLocation(shader_program, "blue_frag_channel");

	if (shader_program <= 0)
	{	
		fprintf(stderr, "ERROR: could not compile shader_program.");
		glfwTerminate();
		return 0;
	}	

	glUseProgram(shader_program);
	glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, camera.view_matrix.m);
	glUniformMatrix4fv(project_matrix_location, 1, GL_FALSE, camera.projection_matrix.m);
	
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

	int selected_sphere = -1;

	while (!glfwWindowShouldClose(window))
	{			
		// Code to limit framerate
		double now = glfwGetTime();		
		delta_time = now - last_update_time;

		if ((now - last_frame_time) >= fps_limit_period)
		{									
			glfwGetWindowSize(window, &window_width, &window_height);
			
			if (true) {
				camera.configure_camera(
					0.1f,
					100.0f,
					67.0f,
					window_width,
					window_height					
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

				// color selected spheres
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

			/* calucate fps */
			double fps = 1 / (now - last_frame_time);
			char temp[256];
			sprintf_s(temp, "FPS %.2lf, delta %f", fps, delta_time);
			glfwSetWindowTitle(window, temp);

			last_frame_time = now;			
		}

		/* Update camera outside FPS loop */
		// TRANSLATIONS		
		float move_delta = camera.camera_speed * (float)delta_time;

		process_movements(move_delta);
		
		if (true) {
			
			// MOUSE PROCESSING
			if (mouse_button_left) {
				
				vec3 ray_world = process_mouse(window);

				// sphere check
				int closest_sphere_clicked = -1;
				float closest_intersection = 0.0f;

				for (int i = 0; i < NUM_OF_SPHERS; i++) {
					float t_distance = 0.0f;

					if (ray_sphere_intersect(camera.camera_position, ray_world, sphere_positions_world[i], sphere_radius, &t_distance)) {

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
		}
		
		last_update_time = now;

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}