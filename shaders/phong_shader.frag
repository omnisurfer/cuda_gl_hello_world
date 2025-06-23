# version 410 core

in vec3 position_eye, normal_eye;

/* https://community.khronos.org/t/sending-an-array-of-structs-to-shader-via-an-uniform-buffer-object/75092 */
/* https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf */

struct Light {	
	// fixed point light properties
	vec3 light_position_world;
    float padding0;
	
	vec3 Ls;
    float padding1;
	
	vec3 Ld;
    float padding2;
	
	vec3 La;
    float padding3;
	
	// surface reflectance
	vec3 Ks;
    float padding4;	
	
	vec3 Kd;
    float padding5;	
	
	vec3 Ka;
	float specular_exponent;
};

uniform mat4 view_matrix;

layout(std140) uniform light_source {
	uniform Light lights[2];
};

out vec4 frag_color;

void main() {

	/*
	Light lights[2];

	lights[0].light_position_world = vec3(0.0, 0.0, 5.0);
	lights[0].Ls = vec3(0.2, 0.2, 0.2);
	lights[0].Ld = vec3(0.7, 0.7, 0.7);
	lights[0].La = vec3(0.1, 0.1, 0.1);
	
	lights[0].Ks = vec3(1.0, 1.0, 1.0);
	lights[0].Kd = vec3(1.0, 0.0, 0.0);
	lights[0].Ka = vec3(1.0, 1.0, 1.0);
	lights[0].specular_exponent = 100.0;
	
	lights[1].light_position_world = vec3(0.0, 0.0, 5.0);
	lights[1].Ls = vec3(0.2, 0.2, 0.2);
	lights[1].Ld = vec3(0.7, 0.7, 0.7);
	lights[1].La = vec3(0.1, 0.1, 0.1);
	
	lights[1].Ks = vec3(1.0, 1.0, 1.0);
	lights[1].Kd = vec3(0.0, 0.25, 0.0);
	lights[1].Ka = vec3(1.0, 1.0, 1.0);
	lights[1].specular_exponent = 100.0;
	*/

    vec3 Ia;
    vec3 Id;
    vec3 Is;
    vec3 n_eye = normalize(normal_eye);
	
    vec3 light_position_eye;
    vec3 distance_to_light_eye;
    vec3 direction_to_light_eye;
	
    vec3 surface_to_viewer_eye;
	
    vec3 half_way_eye;
	
    float dot_product;
    float dot_product_specular;
    float specular_factor;
	
    int number_of_lights = 2;
	
    for (int i = 0; i < number_of_lights; i++)
	{
        Ia += lights[i].La * lights[i].Ka;
        n_eye = normalize(normal_eye);
		
        light_position_eye = vec3(view_matrix * vec4(lights[i].light_position_world, 1.0));
        distance_to_light_eye = light_position_eye - position_eye;
        direction_to_light_eye = normalize(distance_to_light_eye);
		
        dot_product = dot(direction_to_light_eye, n_eye);
        dot_product = max(dot_product, 0.0);
		
        Id += lights[i].Ld * lights[i].Kd * dot_product;
		
        surface_to_viewer_eye = normalize(-position_eye);
		
        half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
        dot_product_specular = max(dot(half_way_eye, n_eye), 0.0);
        specular_factor = pow(dot_product_specular, lights[i].specular_exponent);
		
        Is += lights[i].Ls * lights[i].Ks * specular_factor;
    }

	vec3 Is_final = ( Is ) / number_of_lights;
    vec3 Id_final = ( Id ) / number_of_lights;
    vec3 Ia_final = ( Ia ) / number_of_lights;

	frag_color = vec4( (Is_final) + (Id_final) + (Ia_final), 1.0 );
};