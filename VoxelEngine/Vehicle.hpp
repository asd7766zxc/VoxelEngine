#pragma once
#include "DrawObject.hpp"
#include "SpringSuperGraph.hpp"
class Vehicle:public DrawObject {
public:
	SpringSuperGraph* ssg;
	SpringGraph* wheel;
	Vehicle() {
		ssg = new SpringSuperGraph();
		std::vector<Joint> joints;
		ld thick = 3;
		joints.push_back(Joint{ vec3{0,10,0} });
		for (int i = 0; i <= 10; ++i) {
			ld x = 4 * cos(2 * pi * (i / 10.0));
			ld y = 4 * sin(2 * pi * (i / 10.0)) + 10;
			joints.push_back(Joint{ vec3{x,y,0} });
		}
		joints.push_back(Joint{ vec3{0,10,thick} });
		for (int i = 0; i <= 10; ++i) {
			ld x = 4 * cos(2 * pi * (i / 10.0));
			ld y = 4 * sin(2 * pi * (i / 10.0)) + 10;
			joints.push_back(Joint{ vec3{x,y,thick} });
		}
		wheel = new SpringGraph(joints);
		for (int i = 1; i <= 11; ++i) {
			wheel->addEdge(Spring{ 0,i });
			wheel->addEdge(Spring{ i,(i - 1 + 1) % 11 + 1});
		}
		wheel->addEdge(Spring{ 1,2 });
		for (int i = 13; i <= 23; ++i) {
			wheel->addEdge(Spring{ 12,i });
			wheel->addEdge(Spring{ i, i - 12 });
			wheel->addEdge(Spring{ i,(i - 13 + 1) % 11 + 13 });
		}

		wheel->addEdge(Spring{ 0,12 });
		//wheel->enable_gravity = false;
		ssg->addGraph(wheel);
		ssg->build();
		ssg->U->enable_gravity = false;
	}
	void draw() override {
		ssg->draw();
	}
	void update() override {
		ssg->update();
	}
};