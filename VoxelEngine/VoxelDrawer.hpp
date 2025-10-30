#pragma once
#include "Voxel.hpp"
#include "DrawingUtility.hpp"
#include <GL/glut.h>
#include <iostream>

class VoxelDrawer {
public:
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