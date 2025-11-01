#pragma once
#include <GL/freeglut.h>
#include <utility>

//frame buffer 直接讀取畫面像素，後期處理，然後畫回去
class FrameBuffer {
public:
	int width, height;
	vec2 pos;
	FrameBuffer() {
		
	}
	BYTE buff[1920 * 1080][4];
	BYTE effect_buff[1920 * 1080][4];
	void generate() {
		glReadPixels(0, 0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, buff);
		vec2 cen(200, 200);
		float R = 80;
		for(int i = 0; i < 1080;++i){ //height 
			for (int j = 0; j < 1920; ++j) { //width
				int index = i * 1920 + j;
				vec2 pos(i, j);
				auto &u = effect_buff[index];
				if (abs(pos - cen) > R) {
					for (int g = 0; g < 3; ++g) u[g] = buff[index][g];
					continue;
				}
				ld r = abs(pos - cen);
				ld f = r / R; // 0.5 ~ 1.0

				pos = (pos - cen) * f + cen;
				pos.x = std::max(0, std::min((int)pos.x, 1920));
				pos.y = std::max(0, std::min((int)pos.y, 1080));
				int pi = pos.x * 1920 + pos.y;
				for (int g = 0; g < 3; ++g) u[g] = buff[pi][g];
			}
		}
	}
	void render() {
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 1920, 0, 1080); // to NDC
		glDrawPixels(1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, effect_buff);
		glPopMatrix();
	}
};