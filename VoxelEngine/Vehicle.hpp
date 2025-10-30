#pragma once
#include "GameObject.hpp"
#include "SpringSuperGraph.hpp"
#include "Vertex.hpp"
#include "Wheel.hpp"
#include "Box.hpp"
#include "BoundingBox.hpp"

std::vector<BoundingBox* > BBs;

class Vehicle: public GameObject {
public:
	SpringGraph* G;
	Box box;
	
	//box
	std::vector<Vertex> vertices;
	std::vector<Wheel* > wheels;
	std::vector<Spring> springs;
	ld joint_size = 1.0f;

	ld sx = 2, sy = 1, sz = 4;

	vec3 velo;
	bool toggle_moving = false;

	// car
	// 力作用在 vertices 上，整體移動為合力
	// 力會造成轉動。

	Vehicle() {
		pos = vec3(sx / 2, sy / 2, sz / 2);
		for (auto pt : PrimitiveShape::cube) {
			vertices.push_back({ vec3(pt[0] * sx, pt[1] * sy, pt[2] * sz) - pos });
		}
		std::vector<Joint> joints(8);
		G = new SpringGraph(joints);
		G->addEdge(Spring{ 0,1 });
		G->addEdge(Spring{ 2,3 });
		G->addEdge(Spring{ 4,5 });
		G->addEdge(Spring{ 6,7 });
		G->enable_gravity = false;

		wheels.push_back(new Wheel());
		wheels.push_back(new Wheel());
		wheels.push_back(new Wheel());
		wheels.push_back(new Wheel());

		for (int i = 0; i < 4; ++i) {
			wheels[i]->vert.pos = vertices[PrimitiveShape::face[1][i]].pos + pos;
			springs.push_back(Spring{ i,PrimitiveShape::face[1][i],20,0.1});
		}
	}
	void draw_body() {
		for (int i = 0; i < 6; i++) {
			glBegin(GL_POLYGON);  /* Draw the face */
			glVertex3f(TP(vertices[PrimitiveShape::face[i][0]].pos + pos));
			glVertex3f(TP(vertices[PrimitiveShape::face[i][1]].pos + pos));
			glVertex3f(TP(vertices[PrimitiveShape::face[i][2]].pos + pos));
			glVertex3f(TP(vertices[PrimitiveShape::face[i][3]].pos + pos));
			glEnd();
		}
	}
	void draw_spring() {
		std::vector<Color> color{ {1,0,0},{0,1,0},{0,0,1} };
		for (int i = 0; i < 8; ++i) {
			glColor3f(TC(color[i % 3]));
			glPushMatrix();
			glTranslatef(TP(vertices[i].pos + pos));
			glScalef(.2, .2, .2);
			draw_unisphere();
			glPopMatrix();
		}
		glColor3f(1, 1, 1);
		for (const auto [a, b, k, length, partial] : springs) {
			vec3 posa = wheels[a]->vert.pos;
			vec3 posb = vertices[b].pos + pos;
			glPushMatrix();
			vec3 w = posb - posa;
			//vec3 pos = uni(w) * joint_size + posa;
			glTranslatef(TP(posa));
			alignZTo(w);
			glTranslatef(0, 0, joint_size);
			glScalef(.2, .2, abs(w) - 2 * joint_size);
			draw_unicylind();
			glPopMatrix();
		}
	}
	void applyForce(vec3 _force) {
		for(int i = 0; i < 4; ++i)
		wheels[i]->vert.velocity += _force;
	}
	void drawAxis() {
		std::vector<Color> colors = { {1,0,0},{0,1,0},{0,0,1} };
		std::vector<vec3> axiss;

		for (int i = 0; i < 3; i++) {
			glColor3f(TC(colors[i % 3]));
			auto o = vertices[PrimitiveShape::face[i][0]].pos;
			auto v1 = vertices[PrimitiveShape::face[i][1]].pos;
			auto v2 = vertices[PrimitiveShape::face[i][2]].pos;
			axiss.push_back(uni((v1 - o) ^ (v2 - o)) * -10);
		}
		for (int i = 0; i < axiss.size(); ++i) {
			glColor3f(TC(colors[i % 3]));
			glPushMatrix();
			glTranslatef(TP(pos));
			alignZTo(axiss[i]);
			glScalef(.2, .2, abs(axiss[i]));
			draw_unicylind();
			glPopMatrix();
		}
	}

	void draw() override {
		draw_body();
		//drawAxis();
		for (auto& wheel : wheels) wheel->draw();
		//G->draw();
		draw_spring();
	}
	void updateWheel() {
		for (auto& wheel : wheels) wheel->vert.force += vec3(0, -g, 0);
		for (auto& wheel : wheels) {
			wheel->vert.velocity = wheel->vert.velocity * 0.999; // air friction
		}
		for (auto& wheel : wheels) {
			wheel->vert.velocity += wheel->vert.force * dt;
			wheel->vert.force = vec3();
		}
		for (auto& wheel : wheels) {
			auto& vert = wheel->vert;
			ld yy = terrain_generator->getY(vert.pos.x, vert.pos.z);
			if ((vert.pos + vert.velocity * dt).y - wheel->radius < yy) {
				//wheel->vert.force.y *= -0.2;

				vec3 norm = uni(terrain_generator->getNorm(vert.pos.x, vert.pos.z));
				auto v = vert.velocity;
				vert.velocity = v - norm * (v * norm);

			}
			
			vert.pos += terrain_generator->constrains(vert.pos, vert.velocity * dt);
			wheel->vert.pos += wheel->vert.force * dt;


			auto p = wheel->vert.pos;
			wheel->vert.force.y = 0;

			auto orientation = uni(wheel->vert.velocity ^ vec3(0, 1, 0));

			if(abs(orientation) > 0.9) wheel->zaxi = orientation;

			wheel->vert.pos.y = std::max(p.y, terrain_generator->getY(p.x, p.z) + wheel->radius);
			wheel->vert.force = vec3();
		}
		//pos = wheels[0]->vert.pos;
		for (const auto [a, b, k, length, partial] : springs) {
			vec3 posa = wheels[a]->vert.pos;
			vec3 posb = vertices[b].pos + pos;
			vec3 fd = uni(posb - posa);
			ld x = (length - abs(posa - posb));
			ld f = -(k * x);
			ld aa = f , bb = f;
			wheels[a]->vert.force += fd * f;
			vertices[b].force += fd * -f;
		}
		//wheel->
	}

	vec3 totTorque;
	void update() override {
		for (auto& bx : BBs) {
			while (intersect(bx->box, box)) {
				auto dir = uni(pos - bx->vert.pos);
				pos += dir;
				box.reset();
				for (auto vert : vertices) {
					auto p = vert.pos + pos;
					box.update(p);
				}
			}
		}
		updateWheel();
		for (auto& vert : vertices) {
			auto p = vert.pos + pos;
			ld yy = terrain_generator->getY(p.x, p.z);
			if (abs(p.y - yy) > 1e-4) {
				vert.force += vec3(0, -g, 0);
			}
		}
		vec3 totForce;
		
		std::vector<std::pair<vec3, ld>> rotations;
		for (auto& vert : vertices) {
			totForce += vert.force;
			
			//spin
			totTorque += (vert.pos ^ vert.force);

			vert.force = vec3();
		}

		//printf("%.2f %.2f %.2f\n", TP(totTorque));
		for (auto& vert : vertices) {
			if (abs(totTorque) < 1) continue;
			vert.pos = rotate(vert.pos,uni(totTorque), abs(totTorque) * dt);
		}
			totTorque = vec3();

		//ground adjustment
		velo += totForce * dt;
		auto velotmp = velo * dt * 0.999;
		//pos += velo * dt;
		for (auto vert : vertices) {
			auto p = vert.pos + pos;
			ld yy = terrain_generator->getY(p.x, p.z);
			pos.y += std::max(0.f, -(p.y - yy));
			velotmp = terrain_generator->constrains(p, velotmp);
		}
		pos += velotmp;
		for (auto vert : vertices) {
			auto p = vert.pos + pos;
			ld yy = terrain_generator->getY(p.x, p.z);
			pos.y += std::max(0.f, -(p.y - yy));
		}

		box.reset();

		//construct boundingBox
		for (auto vert : vertices) {
			auto p = vert.pos + pos;
			box.update(p);
		}
		vec3 dv;
		for (auto& bx : BBs) {
			if (intersect(bx->box, box)) {
				auto dir = uni(bx->vert.pos - pos);
				dv += dir * -(velo * dir) * 0.1;
				bx->vert.velocity += dir * (velo * dir) * 0.1;
			}
		}
		for (auto& bx : BBs) {
			while (intersect(bx->box, box)) {
				auto dir = uni(pos - bx->vert.pos);
				pos += dir;
				box.reset();
				for (auto vert : vertices) {
					auto p = vert.pos + pos;
					box.update(p);
				}
			}
		}
		velo += dv;
	}
};