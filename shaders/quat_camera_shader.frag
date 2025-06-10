# version 410 core

in float distance;
uniform float blue_frag_channel = 0.0;
out vec4 frag_color;

void main() {

	frag_color = vec4(1.0, 0.0, blue_frag_channel, 1.0);
	frag_color.xyz *= distance;	
};