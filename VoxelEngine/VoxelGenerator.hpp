#pragma once
#include <vector>
#include "Voxel.hpp"
#include "VoxelDrawer.hpp"
class VoxelGenerator {
	/*float r = 4.0f;
		for (int i = -50; i < 50; ++i) {
			for (int j = -50; j < 50; ++j) {
				float z = sin(i * 0.2) + cos(j * 0.2);
				Voxel vox = { vec3(i * r,r * 4 * z + 10,j * r),r };
				vox.color = {1,1,1};
				vox.color = vox.color * ((z + 2.0) / 4.0);
				voxels.push_back(vox);
			}
		}*/

public:
	std::vector<Voxel> voxels;
	VoxelDrawer* drawer;
	VoxelGenerator() {
		drawer = new VoxelDrawer();
	}
	virtual void generate() {}
	void draw() {
		for (auto voxel : voxels) drawer->drawSingleVoxel(voxel);
	}
};