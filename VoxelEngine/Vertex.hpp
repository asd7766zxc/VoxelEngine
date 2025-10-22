#pragma once
#include "MathUtility.hpp"
struct Vertex {
	vec3 pos;
	vec3 velocity;
	vec3 force;
	ld mass = 1.0f;
};