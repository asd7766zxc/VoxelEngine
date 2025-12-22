#include "GameObject.hpp"
#include "DrawingUtility.hpp"
#include "GL/freeglut.h"
#include "PhysicUtility.hpp"
#include "MathUtility.hpp";

//is behind you
class sphere {

};
class Particle : public GameObject {
public:
	float ang = 0.0f;
	float aspeed = 0.0f;
	float R = 40.0f;
	Particle() {
		R = 100.0f * random_double() + 50.0f;
		aspeed = 0.2 * random_double();
	}
	void draw() override {
		glPushMatrix();
		//glBUff
		glMultMatrixf(localToWorld().transposed().mt);
		draw_unisphere();
		glPopMatrix();
	}
	void update() override {
		pos.x = R * std::cos(ang);
		pos.y = aspeed * 100.0f;
		pos.z = R * std::sin(ang);
		ang += aspeed;
	}
};