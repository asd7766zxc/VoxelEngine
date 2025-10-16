#include "Storages.hpp"
void* new_2d_array(int h, int w, int size) {
	void** p;
	p = (void**)new char[h * sizeof(void*) + h * w * size];
	for (int i = 0; i < h; i++) p[i] = ((char*)(p + h)) + i * w * size;
	return p;
}