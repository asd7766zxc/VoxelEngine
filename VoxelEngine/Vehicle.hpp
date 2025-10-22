#pragma once
#include "DrawObject.hpp"
#include "SpringSuperGraph.hpp"
#include "Vertex.hpp"
#include "Wheel.hpp"

float cube_pts[][3] = { {0, 0, 0},
					   {1, 0, 0},
					   {1, 1, 0},
					   {0, 1, 0},
					   {0, 0, 1},
					   {1, 0, 1},
					   {1, 1, 1},
					   {0, 1, 1} };

int cube_fc[][4] = { {0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5},
					{4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3} };


class Vehicle:public DrawObject {
public:
	SpringGraph* G;
	
	//box
	std::vector<Vertex> vertices;
	std::vector<Wheel* > wheels;
	std::vector<Spring> springs;
	ld joint_size = 1.0f;

	ld sx = 2, sy = 1, sz = 4;
	ld g = 0.5;
	vec3 pos;
	vec3 velo;
	bool toggle_moving = false;

	// car
	// 力作用在 vertices 上，整體移動為合力
	// 力會造成轉動。

	Vehicle() {
		pos = vec3(sx / 2, sy / 2, sz / 2);
		for (auto pt : cube_pts) {
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
		springs.push_back(Spring{ 0,0,1,0.1 });
	}
	void draw_body() {
		for (int i = 0; i < 6; i++) {
			glBegin(GL_POLYGON);  /* Draw the face */
			glVertex3f(TP(vertices[cube_fc[i][0]].pos + pos));
			glVertex3f(TP(vertices[cube_fc[i][1]].pos + pos));
			glVertex3f(TP(vertices[cube_fc[i][2]].pos + pos));
			glVertex3f(TP(vertices[cube_fc[i][3]].pos + pos));
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
		wheels[0]->vert.velocity += _force;
	}
	void draw() override {
		draw_body();
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

			auto orientation = uni(wheel->vert.force ^ vec3(0, 1, 0));

			if(abs(orientation) > 0.9) wheel->zaxi = orientation;

			wheel->vert.pos.y = std::max(p.y, terrain_generator->getY(p.x, p.z) + wheel->radius);
			wheel->vert.force = vec3();
		}
		pos = wheels[0]->vert.pos;
		//for (const auto [a, b, k, length, partial] : springs) {
		//	vec3 posa = wheels[a]->vert.pos;
		//	vec3 posb = vertices[b].pos + pos;
		//	vec3 fd = uni(posb - posa);
		//	ld x = (length - abs(posa - posb));
		//	ld f = -(k * x);
		//	ld aa = f , bb = f;
		//	//wheels[a]->vert.force += fd * f;
		//	vertices[b].force += fd * -f;
		//}
		//wheel->
	}
	void update() override {
		vec3 cen;
		int cen_cnt = 0;
		updateWheel();
		for (auto& vert : vertices) {
			auto p = vert.pos + pos;
			ld yy = terrain_generator->getY(p.x, p.z);
			if (abs(p.y - yy) > 1e-4) {
				vert.force += vec3(0, -g, 0);
			}
		}
		vec3 totForce;
		vec3 totTorque;
		if (cen_cnt == 0) cen = vec3();
		else {
			cen = cen * (1.0f / cen_cnt);
		}
		std::vector<std::pair<vec3, ld>> rotations;
		for (auto& vert : vertices) {
			totForce += vert.force;
			
			//spin
			totTorque += vert.pos ^ vert.force;

			vert.force = vec3();
		}
		auto tmp = rotate(vec3(1, 0, 0), vec3(0, 0, 1), pi / 2);

		//printf("%.2f %.2f %.2f\n", TP(totTorque));
		for (auto& vert : vertices) {
			if (abs(totTorque) < 1) continue;
			vert.pos = rotate(vert.pos,uni(totTorque), abs(totTorque) * dt);
		}

		//ground adjustment
		velo += totForce * dt;
		auto velotmp = velo * dt;
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
	}
};