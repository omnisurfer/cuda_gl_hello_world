# version 410 core

layout(location = 0) in vec3 vertex_position;

uniform mat4 model_matrix, view_matrix, projection_matrix;

out float distance;

void main() {
	
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex_position, 1.0);
	distance = vertex_position.z;
};