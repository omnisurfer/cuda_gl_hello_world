# version 410 core

in float distance;
out vec4 frag_color;

void main() {

	frag_color = vec4(1.0, 0.0, 0.25, 1.0);
	frag_color.xyz *= distance;	
};