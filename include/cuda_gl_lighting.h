#pragma once

#include <cuda_gl_common.h>

// https://stackoverflow.com/questions/70706449/how-to-set-a-uniform-struct-in-glsl-4-6
// https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.html#layout-qualifiers

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