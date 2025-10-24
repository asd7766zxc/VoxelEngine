#pragma once
#include "DrawObject.hpp"
#include "Vertex.hpp"
#include "Box.hpp"
#include <GL/freeglut.h>
#include "GlobalDefinitions.hpp"

class BoundingBox : public DrawObject {
public:
	Vertex vert;
	Box box;
	ld sx = 3, sy = 3, sz = 3;
	std::vector<Vertex> vertices;
	Color color;
	bool isBuilding = false;

	BoundingBox() {
		color = Color{ 1,1,1 };

		vert.pos = vec3(sx / 2, sy / 2, sz / 2);
		for (auto pt : PrimitiveShape::cube) {
			vertices.push_back({ vec3(pt[0] * sx, pt[1] * sy, pt[2] * sz) - vert.pos });
		}
	}
	void draw() override {
		glColor3f(TC(color));
		for (int i = 0; i < 6; i++) {
			glBegin(GL_POLYGON);  /* Draw the face */
			glVertex3f(TP(vertices[PrimitiveShape::face[i][0]].pos + vert.pos));
			glVertex3f(TP(vertices[PrimitiveShape::face[i][1]].pos + vert.pos));
			glVertex3f(TP(vertices[PrimitiveShape::face[i][2]].pos + vert.pos));
			glVertex3f(TP(vertices[PrimitiveShape::face[i][3]].pos + vert.pos));
			glEnd();
		}

	}
	void update() override {
		box.reset();

		//construct boundingBox
		for (auto verts : vertices) {
			auto p = verts.pos + vert.pos;
			box.update(p);
		}
		if (isBuilding) return;
		for (auto& verts : vertices) {
			auto p = verts.pos + vert.pos;
			ld yy = terrain_generator->getY(p.x, p.z);
			if (abs(p.y - yy) > 1e-4) {
				verts.force += vec3(0, -g, 0);
			}
		}
		vec3 totForce;
		vec3 totTorque;

		std::vector<std::pair<vec3, ld>> rotations;
		for (auto& vert : vertices) {
			totForce += vert.force;
			//spin
			totTorque += vert.pos ^ vert.force;

			vert.force = vec3();
		}

		for (auto& vert : vertices) {
			if (abs(totTorque) < 1) continue;
			vert.pos = rotate(vert.pos, uni(totTorque), abs(totTorque) * dt);
		}

		//ground adjustment
		vert.velocity += totForce * dt * 0.999;
		auto velotmp = vert.velocity * dt;
		//pos += velo * dt;
		for (auto verts : vertices) {
			auto p = verts.pos + vert.pos;
			ld yy = terrain_generator->getY(p.x, p.z);
			vert.pos.y += std::max(0.f, -(p.y - yy));
			velotmp = terrain_generator->constrains(p, velotmp);
		}
		vert.pos += velotmp;
		for (auto verts : vertices) {
			auto p = verts.pos + vert.pos;
			ld yy = terrain_generator->getY(p.x, p.z);
			vert.pos.y += std::max(0.f, -(p.y - yy));
		}

		box.reset();

		//construct boundingBox
		for (auto verts : vertices) {
			auto p = verts.pos + vert.pos;
			box.update(p);
		}
	}
};