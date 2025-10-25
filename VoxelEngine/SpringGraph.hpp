#pragma once
#include <vector>
#include <queue>
#include <GL/freeglut.h>
#include "MathUtility.hpp"
#include "Joint.hpp"
#include "Spring.hpp"
#include "PhysicUtility.hpp"
#include "DrawingUtility.hpp"
#include "GlobalDefinitions.hpp"
#include <utility>

class SpringGraph :public GameObject {
public:
	// G(V,E)
	// |V| = n, |E| = m
	int n = 0;
	int m = 0;
	float joint_size = 1.0f;
	std::vector<Joint> joints;
	std::vector<Spring> springs;
	std::vector<std::vector<std::pair<int,int>>> G;

	bool enable_gravity = true;
	ld air_friction = 0.999f;

	SpringGraph(std::vector<Joint>& _joints):
		joints(_joints),n(_joints.size()){
		G.resize(n);
	}
	void addEdge(Spring spring) {
		int a = spring.a;
		int b = spring.b;
		if (!((0 <= a && a < n) && (0 <= b && b < n))) return;
		if (spring.length <= 0) {
			vec3 posa = joints[a].pos;
			vec3 posb = joints[b].pos;
			spring.length = abs(posa - posb);
		}
		G[a].push_back({ b,m });
		G[b].push_back({ a,m });
		springs.push_back(spring);
		++m;
	}
	void update() override {
		if (!enable_gravity) return;
		if (enable_gravity) {
			for (auto& joint : joints) {
				//apply force attenuation
				joint.force = joint.force * air_friction;
				joint.force += vec3(0, -10, 0); // gravity;
			}
		}
		
		//撞地板
		for (auto& joint : joints) {
			ld floor_y = terrain_generator->getY(joint.pos.x, joint.pos.z);
			if (joint.pos.y < floor_y) {
				joint.force += vec3(0, 1, 0) * abs((joint.velocity.y) / dt);
			}
			joint.pos.y = std::max(joint.pos.y, floor_y);
		}

		// F = ma, j0---s---j1
		// 兩端受力相等，且都為 F 但方向相反
		for (const auto [a, b, k, length,partial] : springs) {
			vec3 posa = joints[a].pos;
			vec3 posb = joints[b].pos;
			ld am = joints[a].mass;
			ld bm = joints[b].mass;
			vec3 fd = uni(posb - posa);
			ld x = (length - abs(posa - posb));
			ld f = -(k * x);
			ld aa = f / am,bb = f / bm;
			joints[a].force += fd * f;
			joints[b].force += fd * -f;

			//joints[a].velocity = joints[a].velocity + fd * aa * dt;
			//joints[b].velocity = joints[b].velocity - fd * bb * dt;
		}

		//最後作用在物體上的還是速度
		for (auto& joint : joints) {
			joint.velocity += joint.force * ( 1 / joint.mass) * dt;
			joint.force = vec3(0, 0, 0);
		}
		for (auto& joint : joints) {
			joint.pos = joint.pos + joint.velocity * dt; 
		}
	}

	void draw() override {
		std::vector<Color> color{ {1,0,0},{0,1,0},{0,0,1} };
		for (int i = 0; i < n; ++i) {
			glColor3f(TC(color[i % 3]));
			glPushMatrix();
			glTranslatef(TP(joints[i].pos));
			glScalef(.2, .2, .2);
			draw_unisphere();
			glPopMatrix();
		}
		glColor3f(1, 1, 1);
		for (const auto [a, b, k,length,partial] : springs) {
			vec3 posa = joints[a].pos;
			vec3 posb = joints[b].pos;
			glPushMatrix();
			vec3 w = posb - posa;
			//vec3 pos = uni(w) * joint_size + posa;
			glTranslatef(TP(posa));
			alignZTo(w);
			glTranslatef(0,0,joint_size);
			glScalef(.2 ,.2, abs(w) - 2 * joint_size);
			draw_unicylind();
			glPopMatrix();
		}
	}
};