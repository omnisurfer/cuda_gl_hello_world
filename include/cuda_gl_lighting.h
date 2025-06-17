#pragma once

#include <cuda_gl_common.h>

class CUDAGLLight {

public:
	vec3 light_position_world = vec3(10.0, 10.0, 10.0);
	vec3 light_specular = vec3(1.0, 1.0, 1.0);	// specular color
	vec3 light_diffuse = vec3(0.7, 0.7, 0.7); // diffuse color
	vec3 light_ambient = vec3(0.2, 0.2, 0.2); // ambient color
};

class CUDAGLSurfaceProperties {

public:
	vec3 coefficient_reflection = vec3(1.0, 1.0, 1.0); // specular light reflectance
	vec3 coefficient_diffusion = vec3(1.0, 0.5, 0.0); // diffuse surface reflectance
	vec3 coefficient_ambient = vec3(1.0, 1.0, 1.0); // ambient light reflectance
	float specular_exponent = 100.0; // specular power
};