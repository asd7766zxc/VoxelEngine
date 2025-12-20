#pragma once
#include <GL/freeglut.h>
#include <utility>

//frame buffer 直接讀取畫面像素，後期處理，然後畫回去
class FrameBuffer {
public:
	vec2 cen;
	float R = 100.0f;
	FrameBuffer():cen(200, 200) {
		
	}
	int width = 512;
	int height = 512;
	void setCen(float x, float y) {
		float w = (width / 2.0f);
		float h = (height / 2.0f);
		x = x*w + w;
		y = y*h + h;
		cen = vec2(y,x);
	}
	BYTE buff[1920 * 1080][4];
	BYTE effect_buff[1920 * 1080][4];
	void generate() {
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buff);
		for(int i = 0; i < height;++i){ //height 
			for (int j = 0; j < width; ++j) { //width
				int index = i * width + j;
				if (index < 0 || index >= width * height) continue;
				vec2 pos(i, j);
				auto &u = effect_buff[index];
				if (abs(pos - cen) > R) {
					for (int g = 0; g < 3; ++g) u[g] = buff[index][g];
					continue;
				}
				ld r = abs(pos - cen);
				ld f = r / R; // 0.5 ~ 1.0
				
				pos = (pos - cen) * f + cen;
				pos.x = std::max(0, std::min((int)pos.x, width));
				pos.y = std::max(0, std::min((int)pos.y, height));
				int pi = pos.x * width + pos.y;
				for (int g = 0; g < 3; ++g)  u[g] = buff[pi][g];
			}
		}
	}
	void render() {
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height); // to NDC
		glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, effect_buff);
		glPopMatrix();
	}
};