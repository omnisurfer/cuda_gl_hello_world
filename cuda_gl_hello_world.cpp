/*
* Using nvdia particles CUDA example for guidance.
*/

#define _USE_MATH_DEFINES
#include <cmath>

// OpenGL Graphics includes
/* https://www.glfw.org/documentation.html */
/* https://www.youtube.com/watch?v=uO__ntYT-2Q */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// CUDA
#include <cuda_runtime.h>

// Includes
#include <cstdio>
#include <fstream>
#include <iterator>
#include <vector>
#include <assert.h>

// Utils
#include "stb_image.h"

// CUDA externs
extern "C" int execute_kernel();

GLuint compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename);

/* Create a shader https://antongerdelan.net/opengl/compute.html */
// TBD

static void error_callback_glfw(int error, const char* description) {
	fprintf(stderr, "GLFW ERROR: code %i msg: %s\n", error, description);
}

static void gl_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	else if (key == GLFW_KEY_R) {
		printf("Recompiling shaders\n");

		GLuint shader_program = compile_and_link_shader_program_from_files("shaders\\triangle_shader.vert", "shaders\\triangle_shader.frag");

		if (shader_program > 0)
		{
			glUseProgram(shader_program);
		}
	}
}

GLFWwindow* init_gl() {

	printf("GLFW %s\n", glfwGetVersionString());

	glfwSetErrorCallback(error_callback_glfw);

	GLFWwindow* window;

	if (!glfwInit()) {
		
		fprintf(stderr, "ERROR: could not start GLFW3.\n");
		return 0;
	}

	/* min version */
	/*
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	*/

	/* MSAA */
	glfwWindowHint(GLFW_SAMPLES, 8);

	/* Create a windowed mode window and context */
	window = glfwCreateWindow(640, 480, "CUDA GL Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);	

	/* Start glad for OpenGL functions */
	int version_glad = gladLoadGL(glfwGetProcAddress);

	if (version_glad == 0) {
		fprintf(stderr, "ERROR: Failed to initialize OpenGL context.\n");
		return 0;
	}

	printf("%s\n", glGetString(GL_VERSION));
	printf("%s\n", glGetString(GL_RENDERER));

	// Create callback for key presses
	glfwSetKeyCallback(window, gl_key_callback);
	
	return window;
}

GLuint compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename) {
	
	// read in the triangle shaders	
	std::ifstream input;
	std::vector<unsigned char> buffer;

	input.open(vertex_shader_filename, std::ios::binary);
	buffer.assign(std::istreambuf_iterator<char>(input), {});

	std::string vertex_shader_string = std::string(buffer.begin(), buffer.end());

	input.close();
	buffer.clear();

	const char* vertex_shader = vertex_shader_string.c_str();
	printf("Vertex shader:\n%s", vertex_shader_string.c_str());

	input.open(fragment_shader_filename, std::ios::binary);
	buffer.assign(std::istreambuf_iterator<char>(input), {});

	std::string fragment_shader_string = std::string(buffer.begin(), buffer.end());

	input.close();
	buffer.clear();

	printf("Fragment shader:\n%s", fragment_shader_string.c_str());
	const char* fragment_shader = fragment_shader_string.c_str();

	// shaders	
	GLuint vertex_shader_source = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_source, 1, &vertex_shader, NULL);
	glCompileShader(vertex_shader_source);

	int params = -1;
	glGetShaderiv(vertex_shader_source, GL_COMPILE_STATUS, &params);

	if (GL_TRUE != params) {
		int max_length = 2048, actual_length = 0;
		char vslog[2048];
		glGetShaderInfoLog(vertex_shader_source, max_length, &actual_length, vslog);
		fprintf(stderr, "ERROR: Vertex shader index %u did not compile.\n%s\n", vertex_shader_source, vslog);
		return 1;
	}

	GLuint fragment_shader_source = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_source, 1, &fragment_shader, NULL);
	glCompileShader(fragment_shader_source);

	glGetShaderiv(fragment_shader_source, GL_COMPILE_STATUS, &params);

	if (GL_TRUE != params) {
		int max_length = 2048, actual_length = 0;
		char fslog[2048];
		glGetShaderInfoLog(fragment_shader_source, max_length, &actual_length, fslog);
		fprintf(stderr, "ERROR: Fragment shader index %u did not compile.\n%s\n", fragment_shader_source, fslog);
		return 1;
	}

	// compile into shader kernel
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, fragment_shader_source);
	glAttachShader(shader_program, vertex_shader_source);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &params);

	if (GL_TRUE != params) {
		int max_length = 2048, actual_length = 0;
		char plog[2048];
		glGetProgramInfoLog(shader_program, max_length, &actual_length, plog);
		fprintf(stderr, "ERROR: Could not link shader program GL index %u.\n%s\n", shader_program, plog);
		return 1;
	}

	return shader_program;
}

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

	GLuint shader_program = compile_and_link_shader_program_from_files("shaders\\triangle_shader.vert", "shaders\\triangle_shader.frag");

	if (shader_program > 0)
	{
		glUseProgram(shader_program);
		glBindVertexArray(triangle_vao);
	}


	/* 0 swap immediate 1 sync to monitor */
	glfwSwapInterval(1);

	int degree_step = 0;
	const double fps_limit_frequency = 60.0;
	const double fps_limit_period = 1 / fps_limit_frequency;
	double last_update_time = 0;
	double last_frame_time = 0;

	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK);    // cull back face
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

			/* Animation */
			/*
			float amplitude = 1.0;
			float frequency = 1.0;
			float rad_step = float(2 * M_PI / 180.0f) * degree_step;
			float period = 1 / frequency;

			int degree_step_limit = int(fps_limit_frequency / frequency) * 360;

			float phase = 90.0;
			float phase_rad = float(2 * M_PI / 180.0f) * phase;
			float sin_wave = amplitude * sin(rad_step * frequency);
			float sin_wave_out_of_phase = amplitude * sin(rad_step * frequency + phase);

			double curr_s = glfwGetTime();
			int time_uniform_location = glGetUniformLocation(shader_program, "time");
			if (time_uniform_location == -1) {
				printf("Failed to initialize time uniform");
				break;
			}
			glUniform1f(time_uniform_location, (float)curr_s);

			// glBufferData(GL_ARRAY_BUFFER, number_of_triangle_points * sizeof(float), triangle_points, GL_STATIC_DRAW);

			int cycle_limit = int(fps_limit_frequency / frequency);

			if (degree_step < degree_step_limit) {
				degree_step++;
			}
			else
				degree_step = 0;
			*/
						
			// wire-frame mode
			// glPolygonMode(GL_FRONT, GL_LINE);
			// glPolygonMode(GL_BACK, GL_LINE);
			
			int number_of_triangle_vertices = number_of_colors_points / 3;

			glDrawArrays(GL_TRIANGLES, 0, number_of_triangle_vertices);
			/**/			

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* calucate fps */
			double fps = 1/ (now - last_frame_time);			
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

bool load_cube_map_side(
	GLuint texture,
	GLenum side_target,
	const char* file_name)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int x, y, n;

	// LOAD UP IMAGE DATA TBD
	int force_channels = 4;
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}

	// non-power-of-2 dimensions check, i.e. not square
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name);
	}

	glTexImage2D(
		side_target,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);

	free(image_data);
	return true;
}

/* https://antongerdelan.net/opengl/cubemaps.html */
void create_cube_map(
	const char* front,
	const char* back,
	const char* top,
	const char* bottom,
	const char* left,
	const char* right,
	GLuint* texture_cube) {

	// generate a cube-map texture to hold all the sides
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, texture_cube);

	// load each image
	bool success = load_cube_map_side(*texture_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);	
}

int draw_cube_map(GLFWwindow* window) {

	GLfloat cube_points[] = {
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f,  10.0f
	};
	
	int points_per_triangle = 9;
	int triangles_per_face = 2;
	int faces_per_cube = 6;

	int points_per_cube = points_per_triangle * triangles_per_face * faces_per_cube;

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, points_per_cube * sizeof(GLfloat), &cube_points, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint dummy_texture_cube;

	create_cube_map(
		"Common\\assets\\Yokohama3\\posz.jpg",
		"Common\\assets\\Yokohama3\\posz.jpg",
		"Common\\assets\\Yokohama3\\posz.jpg",
		"Common\\assets\\Yokohama3\\posz.jpg",
		"Common\\assets\\Yokohama3\\posz.jpg",
		"Common\\assets\\Yokohama3\\posz.jpg",
		&dummy_texture_cube
	);

	// DEBUG RETURN
	return 0;

	while (!glfwWindowShouldClose(window))
	{
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

int main(int arc, char** argvv) {

	printf("Starting cuda_gl_hello_world\n");

	GLFWwindow* window;

	window = init_gl();

	if (!window)
	{
		printf("Failed to create Open GL window");
		return -1;
	}

	draw_simple_triangle(window);

	draw_cube_map(window);

	if (false) {
		execute_kernel();

		window = init_gl();

		if (!window)
		{
			printf("Failed to create Open GL window");
			return -1;
		}

		draw_simple_triangle(window);
	}

	return 0;
}