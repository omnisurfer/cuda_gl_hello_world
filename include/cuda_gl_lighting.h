#pragma once

#include <cuda_gl_common.h>

// https://stackoverflow.com/questions/70706449/how-to-set-a-uniform-struct-in-glsl-4-6
// https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.html#layout-qualifiers

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