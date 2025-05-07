# version 410 core

in vec3 vertex_position;
uniform float time;
out vec3 pos;

void main() {
	
	pos = vertex_position;
	pos.y += sin(time);

	gl_Position = vec4(pos, 1.0);
};