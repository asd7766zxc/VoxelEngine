#include "GameObject.hpp"

class Sphere : GameObject {
public:
	float radius;
	float slice;
	Sphere(float radius,float slice) : radius(radius),slice(slice) {}
	void draw() override {

	}
};