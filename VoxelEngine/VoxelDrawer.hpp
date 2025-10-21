#pragma once
#include "Voxel.hpp"
#include "DrawingUtility.hpp"
#include <GL/glut.h>

class VoxelDrawer {
public:
	void drawSingleVoxel(Voxel& voxel) {
		glColor3f(TC(voxel.color));
		glPushMatrix();
		glTranslatef(TP(voxel.pos));
		glScalef(voxel.r, voxel.r, voxel.r);
		draw_unicube();
		glPopMatrix();
	}
};