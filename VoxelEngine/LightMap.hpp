#pragma once
#include "Texture.hpp"
#include "GlobalDefinitions.hpp"
#include "SpotLight.hpp"

class LightMap : public Texture {
public:
	const static int width = 100;
	const static int height = 100;
	BYTE data[width * height * 4];
	SpotLight* light;
	LightMap(){
		allocate(1);
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	void render() override {
		auto tg = terrain_generator;
		for (int i = 0; i < width; ++i) {
			for (int j = 0; j < height; ++j) {
				int ind = (i * height + j) * 4;
				data[ind + 3] = 255;
				float y = tg->pos.z + float(j) / (height - 1) * tg->height;
				float x = tg->pos.x + float(i) / (width - 1) * tg->width;
				auto v = vec3(x,tg->F(x,y), y);
				auto r = uni(v - light->pos);
				auto dir = uni((light->localToWorld() * vec4(0, 0, -1, 0)).toVec3());
				auto c = r * dir;
				if (c >= cos(DegreeToRad(light->cutoff_angle))) {
					data[ind] = data[ind + 1] = data[ind + 2] = 255;
				}
				else {
					data[ind] = data[ind + 1] = data[ind + 2] = 128;
				}
			}
		}
		for (int i = 0; i < width; ++i) {
			for (int j = 0; j <= i; ++j) {
				int ind = (i * height + j) * 4;
				int indi = (j * height + i) * 4;
				std::swap(data[ind +0], data[indi +0]);
				std::swap(data[ind +1], data[indi +1]);
				std::swap(data[ind +2], data[indi +2]);
				std::swap(data[ind +3], data[indi +3]);
			}
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, getTexID(0));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
};