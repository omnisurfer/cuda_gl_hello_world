# version 410 core

const int number_of_lights = 3;

in vec3 position_eye, normal_eye;

/* https://community.khronos.org/t/sending-an-array-of-structs-to-shader-via-an-uniform-buffer-object/75092 */
/* https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf Pg. 137-138 */

struct Light {	
	// fixed point light properties
	vec4 light_position_world;    	
	vec4 Ls;	
	vec4 Ld;	
	vec4 La;
    	
	// surface reflectance
	vec4 Ks;
	vec4 Kd;	
	vec4 Ka;
	float specular_exponent;
    float padding0;
    float padding1;
    float padding2;
};

uniform mat4 view_matrix;

layout(std140) uniform light_source {
    uniform Light lights[number_of_lights];
};

out vec4 frag_color;

void main() {
    
    float dot_product;
    float dot_product_specular;
    float specular_factor;
    
    // vec3
    if (true)
    {
        vec3 Ia;
        vec3 Id;
        vec3 Is;
        vec3 n_eye;
	
        vec3 light_position_eye;
        vec3 distance_to_light_eye;
        vec3 direction_to_light_eye;
	
        vec3 surface_to_viewer_eye;
	
        vec3 half_way_eye;
	
        for (int i = 0; i < number_of_lights; i++)
        {
            Ia += lights[i].La.xyz * lights[i].Ka.xyz;
            n_eye = normalize(normal_eye);
		
            light_position_eye = vec3(view_matrix * vec4(lights[i].light_position_world));
            distance_to_light_eye = light_position_eye - position_eye;
            direction_to_light_eye = normalize(distance_to_light_eye);
		
            dot_product = dot(direction_to_light_eye, n_eye);
            dot_product = max(dot_product, 0.0);
		
            Id += lights[i].Ld.xyz * lights[i].Kd.xyz * dot_product;
		
            surface_to_viewer_eye = normalize(-position_eye);
		
            half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
            dot_product_specular = max(dot(half_way_eye, n_eye), 0.0);
            specular_factor = pow(dot_product_specular, lights[i].specular_exponent);
		
            Is += lights[i].Ls.xyz * lights[i].Ks.xyz * specular_factor;
        }

        vec3 Is_final = (Is) / number_of_lights;
        vec3 Id_final = (Id) / number_of_lights;
        vec3 Ia_final = (Ia) / number_of_lights;

        frag_color = vec4((Is_final * 1.0) + (Id_final * 1.0) + (Ia_final * 1.0), 1.0);
    }    
    // vec4
    else {
    
        vec4 Ia;
        vec4 Id;
        vec4 Is;
        vec4 n_eye;
	
        vec4 light_position_eye;
        vec4 distance_to_light_eye;
        vec4 direction_to_light_eye;
	
        vec4 surface_to_viewer_eye;
	
        vec4 half_way_eye;	
	
        for (int i = 0; i < number_of_lights; i++)
        {
            Ia += lights[i].La * lights[i].Ka;
            // normalizing on vec4 with w at 1.0 causes the calculated normal 
            // direction to not work with the rest of the code for some reason...
            n_eye = normalize(vec4(normal_eye, 0.0));
		
            light_position_eye = view_matrix * lights[i].light_position_world;
            distance_to_light_eye = light_position_eye - vec4(position_eye, 1.0);
            direction_to_light_eye = normalize(distance_to_light_eye);
		
            dot_product = dot(direction_to_light_eye, n_eye);
            dot_product = max(dot_product, 0.0);
		
            Id += lights[i].Ld * lights[i].Kd * dot_product;
		
            surface_to_viewer_eye = normalize(vec4(-position_eye, 1.0));
		
            half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
            dot_product_specular = max(dot(half_way_eye, n_eye), 0.0);
            specular_factor = pow(dot_product_specular, lights[i].specular_exponent);
                        
            Is += lights[i].Ls * lights[i].Ks * specular_factor;
        }

        vec4 Is_final = (Is) / number_of_lights;
        vec4 Id_final = (Id) / number_of_lights;
        vec4 Ia_final = (Ia) / number_of_lights;

        frag_color = (Is_final * 1.0) + (Id_final * 1.0) + (Ia_final * 1.0);
    }    
};