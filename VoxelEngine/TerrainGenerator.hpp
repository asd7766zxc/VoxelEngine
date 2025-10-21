#pragma once
#include <GL/freeglut.h>
#include <functional>
#include "MathUtility.hpp"
#include "DrawObject.hpp"
#include "DrawingUtility.hpp"

//#include "VoxelGenerator.hpp"
class TerrainGenerator:public DrawObject {
public:
	std::function<ld(ld, ld)> F; // (x,z) -> y
	std::function<vec3(ld, ld)> dF; // (x,z) -> (x',y',z') normal vector, perpendicular to terrain itselft
	std::function<Color(ld, ld)> colorF;
	std::vector<ld*> points;
	std::vector<ld*> colors;

	ld width, height;
	int xdim = 0;
	int ydim = 0;
	vec3 pos; // left-up corner
	ld delta;
	TerrainGenerator(auto f, auto df,auto colorf,vec3 _pos,ld w,ld h,ld del = 0.1) :
		F(f), dF(df),colorF(colorf), pos(_pos), width(w), height(h), delta(del) { }
	
	void generate() {
		for (ld y = pos.z; y <= pos.z + height; y += delta) ++ydim;
		for (ld x = pos.x; x <= pos.x + width; x += delta) {
			++xdim;
			for (ld y = pos.z; y <= pos.z + height; y += delta) {
				ld* tmp = new ld[3]{ x, F(x,y) + pos.y, y};
				points.push_back(tmp);
				auto [r, g, b, a] = colorF(x, y);
				ld* col = new ld[3]{r,g,b};
				colors.push_back(col);
			}
		}
	}
	inline int getPos(int i, int j) {
		return i * ydim + j; 
	}
	void draw() override {
		for (int i = 0; i < xdim; ++i) {
			for (int j = 0; j < ydim; ++j) {
				if (i < xdim - 1 && j < ydim - 1) {
					glBegin(GL_POLYGON);
						glColor3fv(colors[getPos(i, j)]);
						glVertex3fv(points[getPos(i, j)]);

						glColor3fv(colors[getPos(i+1, j)]);
						glVertex3fv(points[getPos(i + 1, j)]);

						glColor3fv(colors[getPos(i, j+1)]);
						glVertex3fv(points[getPos(i, j + 1)]);
					glEnd();
				}

				if (i && j) {
					glBegin(GL_POLYGON);

						glColor3fv(colors[getPos(i, j)]);
						glVertex3fv(points[getPos(i, j)]);

						glColor3fv(colors[getPos(i-1, j)]);
						glVertex3fv(points[getPos(i - 1, j)]);
						
						glColor3fv(colors[getPos(i, j-1)]);
						glVertex3fv(points[getPos(i, j - 1)]);
					glEnd();
				}
			}
		}
	}
};