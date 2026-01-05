#pragma once
const int MAX_TEX_COUNT = 256;
class Texture {
public:
	int base = 0;
	int len = 1;
	static unsigned int texes[MAX_TEX_COUNT];
	static int tex_count;
	int getTexID(int index = 0) const {
		return texes[base + index];
	}
};