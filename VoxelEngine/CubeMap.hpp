#pragma once
#include "Texture.hpp"
#include <GL/glut.h>
#include "MathUtility.hpp"
#include "Camera.hpp"

class CubeMap : public Texture {
public:
	shared_ptr<Camera> cam;
	CubeMap() {
		//cam = 
		base = Texture::tex_count;
		Texture::tex_count += 6;
		glGenTextures(6, &Texture::texes[base]);
	}	
	// Orthol projection 因為當距離越遠時主宰的入射光方向的會是和 cube map 距離的
	void generate() {

	}
};