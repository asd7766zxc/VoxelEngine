#pragma once
#include <vector>
#include "MathUtility.hpp"
//註 : 這裡的 velocity 是彈簧圖下 Joints 間的相對速度
struct Joint {
	vec3 pos;
	vec3 velocity;
	vec3 force;
	ld r = 1.0f;
	ld mass = 1.0f;
};