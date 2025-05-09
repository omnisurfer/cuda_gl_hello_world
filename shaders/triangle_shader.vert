# version 410 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform mat4 translation_matrix;
uniform mat4 rotation_matrix;

out vec3 color;

void main() {

	color = vertex_color;
	gl_Position = translation_matrix * rotation_matrix * vec4(vertex_position, 1.0);

};