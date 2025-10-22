#include "DrawObject.hpp"
#include <GL/freeglut.h>
#include "MathUtility.hpp"
#include "Vertex.hpp"
#include "DrawingUtility.hpp"

class Wheel : public DrawObject {
public:
	Vertex vert;
	ld radius = 1.0f;
	vec3 zaxi;
	Wheel() {
		zaxi = vec3(0, 0, 01);
	}
	void draw() override {
		glColor3f(1,0,0);
		glPushMatrix();
		glTranslatef(TP(vert.pos));
		alignZTo(zaxi);
		glutWireTorus(radius/2.0,  /* inner radius */
			radius,  /* outer radius */
			24,   /* divided into 18 segments */
			12);  /* 12 rings */
		glPopMatrix();
	}
	void update() override {

	}
};