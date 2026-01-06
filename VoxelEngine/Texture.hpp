#pragma once
#include <GL/glut.h>
const int MAX_TEX_COUNT = 256;
class Texture {
public:
	int base = 0;
	int len = 1;
	static unsigned int texes[MAX_TEX_COUNT];
	static int tex_count;
	void allocate(int count) {
		base = tex_count;
		len = count;
		glGenTextures(len, &texes[base]);
		tex_count += len;
	}
	int getTexID(int index = 0) const {
		return texes[base + index];
	}
	void Bind() {
		glBindTexture(GL_TEXTURE_2D, getTexID(0));
	}
	virtual void render() = 0;
};