#pragma once
#include "Texture.hpp"
#include "Utilities.hpp"
#include "Camera.hpp"

class billboard {
public:
	shared_ptr<Texture> tex;
	vec3 pos;
	vec3 norm;
	float size = 10.0f;
	float angle = 0.0f;
	bool rand = false;
	billboard(shared_ptr<Texture> tex) :
		tex(tex) {

	}

	void draw(Camera *cam) {
		auto w = uni(norm);
		auto v = vec3(0, 1, 0);
		auto u = uni(v ^ w);
		v = w ^ u;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		tex->Bind();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glPushMatrix();
		if (rand) {
			glTranslatef(pos.x + random_double() * size / 2.0f, pos.y + random_double() * size / 2.0f, pos.z + random_double() * size / 2.0f);
		}
		else {
			glTranslatef(pos.x, pos.y, pos.z);
		}
		glRotatef(angle, w.x,w.y,w.z);
		glColor4f(1, 1, 1, 0);
		glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);
			glVertex3fv(u * size - v * size);
			glTexCoord2f(1, 0);
			glVertex3fv(- u * size - v * size);
			glTexCoord2f(1, 1);
			glVertex3fv(- u * size + v * size);
			glTexCoord2f(0, 1);
			glVertex3fv(u * size + v * size);
		glEnd();
		glPopMatrix();
	}
};