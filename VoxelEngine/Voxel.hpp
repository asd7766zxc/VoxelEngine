#pragma once
#include "MathUtility.hpp"
#include "DrawingUtility.hpp"
// (0,0,0) (r,0,0) ... + pos 
struct Voxel {
	vec3 pos;
	Color color;
	ld r;
	Voxel(vec3 _pos,ld _r):pos(_pos),r(_r){}
	Voxel() :r(1){}
};