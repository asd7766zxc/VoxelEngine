#pragma once
#include "Texture.hpp"
#include "Utilities.hpp"
#include "Camera.hpp"

class billboard {
public:
	shared_ptr<Texture> tex;
	vec3 pos;
	float size = 10.0f;
	billboard(shared_ptr<Texture> tex) :
		tex(tex) {

	}

	void draw(Camera *cam) {
		auto w = uni(cam->view.transposed().z_axis());
		auto v = vec3(0, 1, 0);
		auto u = uni(v ^ w);
		v = w ^ u;
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glAlphaFunc(GL_GREATER, 0);
		tex->Bind();

		glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);
			glVertex3fv(pos + u * size);
			glTexCoord2f(1, 0);
			glVertex3fv(pos - u * size);
			glTexCoord2f(1, 1);
			glVertex3fv(pos - u * size + v * size);
			glTexCoord2f(0, 1);
			glVertex3fv(pos + u * size + v * size);
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_TEXTURE_2D);
	}
};