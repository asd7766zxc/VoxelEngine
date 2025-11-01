#pragma once
#include "MathUtility.hpp"
#include <utility>

struct BoundingBox {
	vec3 min;
	vec3 max;
	void reset() {
		auto& [x, y, z] = min;
		auto& [mx, my, mz] = max;

		x = y = z = infinity;
		mx = my = mz = -infinity;
	}
	void update(vec3 p){
		auto& [x, y, z] = min;
		auto& [mx, my, mz] = max;
		
		x = std::min(x, p.x);
		y = std::min(y, p.y);
		z = std::min(z, p.z);

		mx = std::max(mx, p.x);
		my = std::max(my, p.y);
		mz = std::max(mz, p.z);

	}
	//AABB
	friend bool intersect(BoundingBox a, BoundingBox b) {
		return (a.min.x <= b.max.x &&
				a.max.x >= b.min.x &&
				a.min.y <= b.max.y &&
				a.max.y >= b.min.y &&
				a.min.z <= b.max.z &&
				a.max.z >= b.min.z);
	}
};