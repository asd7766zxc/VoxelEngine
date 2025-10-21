#pragma once
#include <vector>
#include "Spring.hpp"
#include "SpringGraph.hpp"
#include "DrawObject.hpp"
#include <map>

class SpringSuperGraph:public DrawObject {
public:
	SpringGraph* U;
	std::vector<Joint> joints;
	std::map<SpringGraph*, int> graphs;
	std::vector<SpringGraph*> G;
	int t = 0;
	void update() override {
		U->update();
	}
	void draw() override {
		U->draw();
	}
	void addGraph(SpringGraph* _graph) {
		graphs[_graph] = t;
		G.push_back(_graph);
		t += _graph->n;
	}
	void addEdge(SpringGraph* a, SpringGraph* b, Spring spring) {
		if (!graphs.count(a)) return;
		if (!graphs.count(b)) return;

		spring.a += graphs[a];
		spring.b += graphs[b];

		U->addEdge(spring);

	}
	void build() {
		for (const auto g : G) {
			for (auto joint : g->joints) {
				joints.push_back(joint);
			}
		}
		U = new SpringGraph(joints);
		for (const auto g : G) {
			for (auto edg : g->springs) {
				edg.a += graphs[g];
				edg.b += graphs[g];
				U->addEdge(edg);
			}
		}
	}
};