#pragma once
#include "GameObject.hpp"
#include "DrawingUtility.hpp"
#include "MathUtility.hpp"
#include <GL/freeglut.h>
class SpotLight : public GameObject {
public:
	GLenum light_id = GL_LIGHT7;
	bool light_on = true;
	Color light_color = Color(0.9, 0.9, 0 ,1.0);
	float cutoff_angle = 30.0f;
	float angle = 0.0;
	SpotLight(GLenum light_id) : light_id(light_id) {}

	void draw_light() {
		if (light_on) glEnable(light_id);
		else glDisable(light_id);
		glLightfv(light_id, GL_POSITION, vec4(0, 0, 0,1));
		glLightfv(light_id, GL_SPOT_DIRECTION, vec4(0,0,-1, 0));
		glLightfv(light_id, GL_DIFFUSE, light_color);
		glLightfv(light_id, GL_SPECULAR, light_color);
		glLightf(light_id, GL_SPOT_EXPONENT, 1.0f);
		glLightf(light_id, GL_SPOT_CUTOFF, cutoff_angle);
		glLightf(light_id, GL_CONSTANT_ATTENUATION, 1.0f);
	}
	void drawLightCap() {
		applyColorMaterials({ 0,0,0 }, { 0.5,0.5,0.5 }, { 0.5,0.5,0.5 }, {0,0,0},6);
		float radius = 1.5f;
		int slice = 64;
		auto ori = vec3(0, 0, 2);
		for (int i = 0; i <= slice; ++i) {
			auto getPoint = [&](int ind) {
				float theta = ind * 2.0f * pi / slice;
				return vec3(radius * cos(theta), radius * sin(theta), 0);
			};
			auto a = getPoint(i);
			auto b = getPoint((i + 1) % (slice + 1));

			auto getNorm = [](vec3 a, vec3 b) {
				return (abs2(a) / abs2(b)) * b - a;
			};
			
			glBegin(GL_POLYGON);
				glNormal3fv(getNorm(-ori, a));
				glVertex3fv(a);
				
				glNormal3fv(getNorm(-ori, b));
				glVertex3fv(b);

				glNormal3fv(getNorm(-ori, b));
				glVertex3fv((b - ori) * 0.4 + ori);
				
				glNormal3fv(getNorm(-ori, a));
				glVertex3fv((a - ori) * 0.4 + ori);
			glEnd();
		}
	}
	float animation_time = 100.0f;
	float t = 100.0f;
	void drawLightVolume() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		t -= 0.9f;
		auto colors = light_color;
		colors.a = 0.4f * (t / animation_time);
		applyColorMaterials({ 0,0,0,0 }, colors, { 0,0,0,0 }, { 0,0,0,0 }, 0);
		int slice = 64;
		auto ori = vec3(0, 0, 0);
		float dis = 40.0f;
		float radius = dis * tan(DegreeToRad(cutoff_angle));
		for (int i = 0; i <= slice; ++i) {
			auto getPoint = [&](int ind) {
				float theta = ind * 2.0f * pi / slice;
				return vec3(radius * cos(theta), radius * sin(theta), -dis);
				};
			auto a = getPoint(i);
			auto b = getPoint((i + 1) % (slice + 1));

			auto getNorm = [](vec3 a, vec3 b) {
				return (abs2(a) / abs2(b)) * b - a;
				};

			glBegin(GL_POLYGON);
			glNormal3fv(getNorm(-ori, a));
			glVertex3fv(a);

			glNormal3fv(getNorm(-ori, b));
			glVertex3fv(b);

			glNormal3fv(getNorm(-ori, b));
			glVertex3fv((b - ori) * 0.1 + ori);

			glNormal3fv(getNorm(-ori, a));
			glVertex3fv((a - ori) * 0.1 + ori);
			glEnd();
		}
	}
	void draw() override {
		glPushMatrix();
			glMultMatrixf(localToWorld().transposed().mt);
			draw_light();
			applyColorMaterials({ 0,0,0 }, { 0,0,0 }, { 0,0,0 }, light_color, 0);
			draw_unisphere();
			drawLightCap();
			if(t >= 0)
			drawLightVolume();
		glPopMatrix();
	}
	void update() override {

	}
};