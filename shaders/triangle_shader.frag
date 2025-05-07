# version 410 core

in vec3 pos;
uniform float time;
out vec4 frag_color;

void main() {
	float wild = sin(time) * 0.5 + 0.5;

	frag_color.rba = vec3(1.0);
	frag_color.g = wild;	

	// frag_color = vec4(pos, 1.0);
};