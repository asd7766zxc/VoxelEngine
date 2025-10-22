#include "DrawObject.hpp"
#include "Vertex.hpp"
#include <GL/freeglut.h>

class BoundingBox : public DrawObject {
	Vertex vert;
	ld sx = 1, sy = 0.5, sz = 2;
	std::vector<std::vector<ld>> face;
	std::vector<Vertex> vertices;
	

	BoundingBox() {
		std::vector<std::vector<ld>> cube = { 
					{0, 0, 0},
					{1, 0, 0},
					{1, 1, 0},
					{0, 1, 0},
					{0, 0, 1},
					{1, 0, 1},
					{1, 1, 1},
					{0, 1, 1} };

		face =	{ {0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5},
				{4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3} };

		vert.pos = vec3(sx / 2, sy / 2, sz / 2);
		for (auto pt : cube) {
			vertices.push_back({ vec3(pt[0] * sx, pt[1] * sy, pt[2] * sz) - vert.pos });
		}
	}
	void draw() override {
		for (int i = 0; i < 6; i++) {
			glBegin(GL_POLYGON);  /* Draw the face */
			glVertex3f(TP(vertices[face[i][0]].pos + vert.pos));
			glVertex3f(TP(vertices[face[i][1]].pos + vert.pos));
			glVertex3f(TP(vertices[face[i][2]].pos + vert.pos));
			glVertex3f(TP(vertices[face[i][3]].pos + vert.pos));
			glEnd();
		}

	}
	void update() override {

	}
};