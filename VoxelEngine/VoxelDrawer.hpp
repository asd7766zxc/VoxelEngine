#pragma once
#include "Voxel.hpp"
#include "DrawingUtility.hpp"
#include <GL/glut.h>
#include <iostream>

class VoxelDrawer {
public:
    void drawAxis() {
        std::vector<Color> colors = { {1,0,0},{0,1,0},{0,0,1} };
        std::vector<vec3> axiss;

        for (int i = 0; i < 3; i++) {
            glColor3f(TC(colors[i % 3]));
            auto u = vec3(TC(colors[i])) * 10;
            axiss.push_back(u);
        }
        for (int i = 0; i < axiss.size(); ++i) {
            glColor3f(TC(colors[i % 3]));
            glPushMatrix();
            alignZTo(axiss[i]);
            glScalef(.2, .2, abs(axiss[i]));
            draw_unicylind();
            glPopMatrix();
        }
    }

	void drawSingleVoxel(Voxel& voxel) {
		glColor3f(TC(voxel.color));
		glPushMatrix();

            glPushMatrix();
                glMultMatrixf(voxel.toWorldNoScale().transposed().mt);
                drawAxis();
            glPopMatrix();

			glMultMatrixf(voxel.localToWorld().transposed().mt);
            
			draw_unicube();
		glPopMatrix();
	}
};