#pragma once
#include "MathUtility.hpp"
#include "DrawingUtility.hpp"
#include "GameObject.hpp"
// (0,0,0) (r,0,0) ... + pos 

class Voxel : public GameObject {
public:
	Color color;
	ld r;
	Voxel(vec3 _pos, ld _r) : r(_r) {
		pos = _pos;
	}
	Voxel() :r(1) {}
};