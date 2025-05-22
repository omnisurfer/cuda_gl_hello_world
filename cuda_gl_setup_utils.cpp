#include "cuda_gl_setup_utils.h"

GLFWwindow* init_gl(int window_height, int window_width) {

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
	window = glfwCreateWindow(1024, 1024, "CUDA GL Hello World", NULL, NULL);

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

void gl_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

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

void error_callback_glfw(int error, const char* description) {
	fprintf(stderr, "GLFW ERROR: code %i msg: %s\n", error, description);
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
	printf("Vertex shader:\n%s\n", vertex_shader_string.c_str());

	input.open(fragment_shader_filename, std::ios::binary);
	buffer.assign(std::istreambuf_iterator<char>(input), {});

	std::string fragment_shader_string = std::string(buffer.begin(), buffer.end());

	input.close();
	buffer.clear();

	printf("Fragment shader:\n%s\n", fragment_shader_string.c_str());
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