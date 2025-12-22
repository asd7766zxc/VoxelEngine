#pragma once
#include "GameObject.hpp"
#include "Portal.hpp"

class Tunnel :public GameObject {
public:
	float width = 10.f;
	float height = 10.f;
	float length = 50.f;
	float thickness = 1.f;
	Tunnel()  {
		
	}

	void draw() override {
		glPushMatrix();
		glMultMatrixf(localToWorld().transposed().mt);
		applyColorMaterials({ 0,0,0,0 }, {0.5,0.5,0.5}, {1,1,1}, {0,0,0,0}, 64.0);
		std::vector<vec3> pthick = {
			vec3(-width/2 - thickness, -height/2 - thickness, 0),
			vec3( width/2 + thickness, -height/2 - thickness, 0),
			vec3( width/2 + thickness,  height/2 + thickness, 0),
			vec3(-width/2 - thickness,  height/2 + thickness, 0),
		};
		std::vector<vec3> pthick_to = {
			vec3(-width / 2 - thickness, -height / 2 - thickness, length),
			vec3( width / 2 + thickness, -height / 2 - thickness, length),
			vec3( width / 2 + thickness,  height / 2 + thickness, length),
			vec3(-width / 2 - thickness,  height / 2 + thickness, length),
		};
		std::vector<vec3> pthin = {
			vec3(-width / 2, -height / 2, 0),
			vec3( width / 2, -height / 2, 0),
			vec3( width / 2,  height / 2, 0),
			vec3(-width / 2,  height / 2, 0),
		};
		std::vector<vec3> pthin_to = {
			vec3(-width / 2, -height / 2, length),
			vec3( width / 2, -height / 2, length),
			vec3( width / 2,  height / 2, length),
			vec3(-width / 2,  height / 2, length),
		};
		for (int i = 0; i < 4; ++i) {
			int j = (i + 1) % 4;
			glNormal3fv(-ori(pthick[i], pthick[j], pthick_to[j]));
			glBegin(GL_POLYGON);
				glVertex3fv(pthick[i]);
				glVertex3fv(pthick[j]);
				glVertex3fv(pthick_to[j]);
				glVertex3fv(pthick_to[i]);
			glEnd();

			glNormal3fv(-ori(pthin[i], pthin[j], pthin_to[j]));
			glBegin(GL_POLYGON);
				glVertex3fv(pthin[i]);
				glVertex3fv(pthin[j]);
				glVertex3fv(pthin_to[j]);
				glVertex3fv(pthin_to[i]);
			glEnd();

			glNormal3fv(-ori(pthick[i], pthin[i], pthick[j]));
			glBegin(GL_POLYGON);
				glVertex3fv(pthick[i]);
				glVertex3fv(pthick[j]);
				glVertex3fv(pthin[j]);
				glVertex3fv(pthin[i]);
			glEnd();

			glNormal3fv(-ori(pthick_to[i], pthin_to[i], pthick_to[j]));
			glBegin(GL_POLYGON);
			glVertex3fv(pthick_to[i]);
			glVertex3fv(pthick_to[j]);
			glVertex3fv(pthin_to[j]);
			glVertex3fv(pthin_to[i]);
			glEnd();
		}
		glPopMatrix();
	}
	void update() override {

	}
};