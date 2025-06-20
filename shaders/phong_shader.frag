# version 410 core

in vec3 position_eye, normal_eye;

struct Light {	
	// fixed point light properties
	vec3 light_position_world;
	vec3 Ls;
	vec3 Ld;
	vec3 La;

	// surface reflectance
	vec3 Ks;
	vec3 Kd;
	vec3 Ka;
	float specular_exponent;
};

// fixed point light properties
vec3 light_position_world = vec3(0.0, 0.0, 5.0);
vec3 Ls = vec3(0.2, 0.2, 0.2);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(0.1, 0.1, 0.1);

// surface reflectance
vec3 Ks = vec3(1.0, 1.0, 1.0);
vec3 Kd = vec3(1.0, 0.0, 0.0);
vec3 Ka = vec3(1.0, 1.0, 1.0);
float specular_exponent = 100.0;

uniform mat4 view_matrix;

out vec4 frag_color;

void main() {

	Light lights[2];

	lights[0].light_position_world = vec3(0.0, 0.0, 5.0);
	lights[0].Ls = vec3(0.2, 0.2, 0.2);
	lights[0].Ld = vec3(0.7, 0.7, 0.7);
	lights[0].La = vec3(0.1, 0.1, 0.1);
	lights[0].Ks = vec3(1.0, 1.0, 1.0);
	lights[0].Kd = vec3(1.0, 0.0, 0.0);
	lights[0].Ka = vec3(1.0, 1.0, 1.0);
	lights[0].specular_exponent = 100.0;

	lights[1].light_position_world = vec3(0.0, 0.0, -5.0);
	lights[1].Ls = vec3(0.2, 0.2, 0.2);
	lights[1].Ld = vec3(0.7, 0.7, 0.7);
	lights[1].La = vec3(0.1, 0.1, 0.1);
	lights[1].Ks = vec3(1.0, 1.0, 1.0);
	lights[1].Kd = vec3(0.0, 1.0, 0.0);
	lights[1].Ka = vec3(1.0, 1.0, 1.0);
	lights[1].specular_exponent = 100.0;

	Light light_a;
	light_a.light_position_world = vec3(0.0, 0.0, 5.0);
	light_a.Ls = vec3(0.2, 0.2, 0.2);
	light_a.Ld = vec3(0.7, 0.7, 0.7);
	light_a.La = vec3(0.1, 0.1, 0.1);
	light_a.Ks = vec3(1.0, 1.0, 1.0);
	light_a.Kd = vec3(1.0, 0.0, 0.0);
	light_a.Ka = vec3(1.0, 1.0, 1.0);
	light_a.specular_exponent = 100.0;

	Light light_b;
	light_b.light_position_world = vec3(0.0, 0.0, -5.0);
	light_b.Ls = vec3(0.2, 0.2, 0.2);
	light_b.Ld = vec3(0.7, 0.7, 0.7);
	light_b.La = vec3(0.1, 0.1, 0.1);
	light_b.Ks = vec3(1.0, 1.0, 1.0);
	light_b.Kd = vec3(0.0, 1.0, 0.0);
	light_b.Ka = vec3(1.0, 1.0, 1.0);
	light_b.specular_exponent = 100.0;

	// ambient intensity
	vec3 Ia = lights[0].La * lights[0].Ka;
	vec3 n_eye = normalize( normal_eye );

	// diffuse intensity
	vec3 light_position_eye = vec3( view_matrix * vec4( lights[0].light_position_world, 1.0 ) );
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize( distance_to_light_eye );

	float dot_product = dot( direction_to_light_eye, n_eye );
	dot_product = max(dot_product, 0.0);

	vec3 Id = lights[0].Ld * lights[0].Kd * dot_product;

	// specular intensity
	vec3 surface_to_viewer_eye = normalize ( -position_eye );

	vec3 half_way_eye = normalize( surface_to_viewer_eye + direction_to_light_eye );
	float dot_product_specular = max( dot ( half_way_eye, n_eye ), 0.0 );
	float specular_factor = pow( dot_product_specular, specular_exponent );

	vec3 Is = lights[0].Ls * lights[0].Ks * specular_factor;

	/* */

	// ambient intensity
	vec3 Ia_a = light_b.La * light_b.Ka;

	// diffuse intensity
	vec3 light_position_eye_a = vec3( view_matrix * vec4( lights[1].light_position_world, 1.0 ) );
	vec3 distance_to_light_eye_a = light_position_eye_a - position_eye;
	vec3 direction_to_light_eye_a = normalize( distance_to_light_eye_a );

	float dot_product_a = dot( direction_to_light_eye_a, n_eye );
	dot_product_a = max(dot_product_a, 0.0);

	vec3 Id_a = lights[1].Ld * lights[1].Kd * dot_product_a;

	// specular intensity
	vec3 surface_to_viewer_eye_a = normalize ( -position_eye );

	vec3 half_way_eye_a = normalize( surface_to_viewer_eye_a + direction_to_light_eye_a );
	float dot_product_specular_a = max( dot ( half_way_eye_a, n_eye ), 0.0 );
	float specular_factor_a = pow( dot_product_specular_a, light_b.specular_exponent );

	vec3 Is_a = lights[1].Ls * lights[1].Ks * specular_factor_a;

	vec3 Is_final = ( Is + Is_a ) / 2.0;
	vec3 Id_final = ( Id + Id_a ) / 2.0;
	vec3 Ia_final = ( Ia + Ia_a ) / 2.0;

	frag_color = vec4( (Is_final) + (Id_final) + (Ia_final), 1.0 );
};