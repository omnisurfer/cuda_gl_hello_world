#include <cuda_gl_common.h>

static void debug_gl_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
) {
	
	switch (source) {
	case 0x8246:
		printf("API ");
		break;
	}

	switch(type) {
	case 0x824C:
		printf("ERROR ");
		break;
	}

	switch (severity) {
	case 0x9146:
		printf("HIGH ");
	}

	printf("source 0x%x type 0x%x severity 0x%x\n", source, type, severity);
	printf("length %i message %s userParam %i\n", length, message, *(int*)userParam);
}

GLFWwindow* CUDAGLCommon::init_gl(int window_width, int window_height) {

	printf("GLFW %s\n", glfwGetVersionString());

	glfwSetErrorCallback(CUDAGLCommon::error_callback_glfw);

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
	// enable debug messages
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	/* Create a windowed mode window and context */
	window = glfwCreateWindow(window_width, window_height, "CUDA GL Hello World", NULL, NULL);
	
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

	glfwSetWindowAspectRatio(window, window_width, window_height);

	// center the window
	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* vid_mode = glfwGetVideoMode(primary_monitor);

	glfwSetWindowPos(window, (vid_mode->width - window_width) / 2, (vid_mode->height - window_height) / 2);

	if (true) {
		/**/
		if (GL_KHR_debug) {
			int param = -1;
			printf("KHR debug extension found!\n");
			glDebugMessageCallback(debug_gl_callback, &param);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			printf("debug callback enabled\n");
		}
		else {
			printf("KHR debug extension not avaialbe.\n");
		}
	}

	return window;
}

void CUDAGLCommon::update_shaders(CUDAGLCamera camera) {

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

GLuint CUDAGLCommon::compile_and_link_shader_program_from_files(const char* vertex_shader_filename, const char* fragment_shader_filename) {

	// read in the triangle shaders	
	std::ifstream input;
	std::vector<unsigned char> buffer;

	input.open(vertex_shader_filename, std::ios::binary);
	buffer.assign(std::istreambuf_iterator<char>(input), {});

	std::string vertex_shader_string = std::string(buffer.begin(), buffer.end());

	input.close();
	buffer.clear();

	const char* vertex_shader = vertex_shader_string.c_str();
	// printf("Vertex shader:\n%s\n", vertex_shader_string.c_str());

	input.open(fragment_shader_filename, std::ios::binary);
	buffer.assign(std::istreambuf_iterator<char>(input), {});

	std::string fragment_shader_string = std::string(buffer.begin(), buffer.end());

	input.close();
	buffer.clear();

	// printf("Fragment shader:\n%s\n", fragment_shader_string.c_str());
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