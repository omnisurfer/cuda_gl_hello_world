# version 410 core

in vec3 position_eye, normal_eye;

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

// fixed point light properties
vec3 light_position_world_a = vec3(0.0, 0.0, -5.0);
vec3 Ls_a = vec3(0.2, 0.2, 0.2);
vec3 Ld_a = vec3(0.7, 0.7, 0.7);
vec3 La_a = vec3(0.1, 0.1, 0.1);

// surface reflectance
vec3 Ks_a = vec3(1.0, 1.0, 1.0);
vec3 Kd_a = vec3(0.0, 1.0, 0.0);
vec3 Ka_a = vec3(1.0, 1.0, 1.0);
float specular_exponent_a = 100.0;

uniform mat4 view_matrix;

out vec4 frag_color;

void main() {

	// ambient intensity
	vec3 Ia = La * Ka;	
	vec3 n_eye = normalize( normal_eye );

	// diffuse intensity
	vec3 light_position_eye = vec3( view_matrix * vec4( light_position_world, 1.0 ) );
	vec3 distance_to_light_eye = light_position_eye - position_eye;
	vec3 direction_to_light_eye = normalize( distance_to_light_eye );

	float dot_product = dot( direction_to_light_eye, n_eye );
	dot_product = max(dot_product, 0.0);

	vec3 Id = Ld * Kd * dot_product;

	// specular intensity
	vec3 surface_to_viewer_eye = normalize ( -position_eye );

	vec3 half_way_eye = normalize( surface_to_viewer_eye + direction_to_light_eye );
	float dot_product_specular = max( dot ( half_way_eye, n_eye ), 0.0 );
	float specular_factor = pow( dot_product_specular, specular_exponent );

	vec3 Is = Ls * Ks * specular_factor;

	/* */

	// ambient intensity
	vec3 Ia_a = La_a * Ka_a;	

	// diffuse intensity
	vec3 light_position_eye_a = vec3( view_matrix * vec4( light_position_world_a, 1.0 ) );
	vec3 distance_to_light_eye_a = light_position_eye_a - position_eye;
	vec3 direction_to_light_eye_a = normalize( distance_to_light_eye_a );

	float dot_product_a = dot( direction_to_light_eye_a, n_eye );
	dot_product_a = max(dot_product_a, 0.0);

	vec3 Id_a = Ld_a * Kd_a * dot_product_a;

	// specular intensity
	vec3 surface_to_viewer_eye_a = normalize ( -position_eye );

	vec3 half_way_eye_a = normalize( surface_to_viewer_eye_a + direction_to_light_eye_a );
	float dot_product_specular_a = max( dot ( half_way_eye_a, n_eye ), 0.0 );
	float specular_factor_a = pow( dot_product_specular_a, specular_exponent_a );

	vec3 Is_a = Ls_a * Ks_a * specular_factor_a;

	vec3 Is_final = ( Is + Is_a ) / 2.0;
	vec3 Id_final = ( Id + Id_a ) / 2.0;
	vec3 Ia_final = ( Ia + Ia_a ) / 2.0;

	frag_color = vec4( (Is_final) + (Id_final) + (Ia_final), 1.0 );
};