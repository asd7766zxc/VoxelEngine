#pragma once
#include "MathUtility.hpp"
class ray {
public:
	ray(){}
	vec3 orig,dir;
	ray(vec3 origin, vec3 direction) : orig(origin), dir(direction) {}

	vec3 at(float t) const {
		return orig + (dir * t);
	}

};