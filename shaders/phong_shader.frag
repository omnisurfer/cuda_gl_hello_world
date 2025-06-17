# version 410 core

in vec3 position_eye, normal_eye;

// fixed point light properties
vec3 light_position_world = vec3(0.0, 0.0, 2.0);
vec3 Ls = vec3(1.0, 1.0, 1.0);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(1.0, 0.2, 0.2);

// surface reflectance
vec3 Ks = vec3(1.0, 1.0, 1.0);
vec3 Kd = vec3(1.0, 0.5, 0.0);
vec3 Ka = vec3(1.0, 1.0, 1.0);
float specular_exponent = 100.0;

uniform float blue_frag_channel = 0.0;
uniform mat4 view_matrix;

out vec4 frag_color;

void main() {

	vec3 light_position_eye = vec3(view_matrix * vec4(light_position_world, 1.0));
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize(distance_to_light_eye);

	float dot_product = dot(direction_to_light_eye, normal_eye);
	dot_product = max(dot_product, 0.0);

	// ambient intensity
	vec3 Ia = La * Ka;

	// diffuse intensity
	vec3 Id = Ld * Kd * dot_product;

	// specular intensity
	vec3 Is = vec3(0.0, 0.0, 0.0);

	frag_color = vec4(Is + Id + Ia, 1.0);	
};