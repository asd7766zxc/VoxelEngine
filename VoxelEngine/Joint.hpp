#pragma once
#include <vector>
#include "MathUtility.hpp"
//�� : �o�̪� velocity �O�u®�ϤU Joints �����۹�t��
struct Joint {
	vec3 pos;
	vec3 velocity;
	vec3 force;
	ld r = 1.0f;
	ld mass = 1.0f;
};