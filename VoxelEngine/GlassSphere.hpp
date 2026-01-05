#pragma once
#include "GameObject.hpp"
#include <GL/glut.h>
#include "Camera.hpp"

class GlassSphere : public GameObject {
	public:
	float radius = 1.0f;
	float slice = 10;
	Camera* cam;
	GlassSphere(float r,Camera* camera) :radius(r),cam(camera) {
		scale = vec3(r, r, r);
	}

	void draw() override {
		glPushMatrix();

		auto mat = cam->Matrix() * localToWorld();
		glMultMatrixf(localToWorld().transposed().mt);
		for (int h = 0; h < slice; ++h) {
			for (int i = 0; i < slice; ++i) {
				float nh = cos(float(h) / slice * pi);
				float nr = sin(float(h) / slice * pi);

				float theta1 = float(i) / slice * 2 * pi;
				float theta2 = float(i + 1) / slice * 2 * pi;
				auto v1 = vec3(nr * cos(theta1), nr * sin(theta1), nh);
				auto v2 = vec3(nr * cos(theta2), nr * sin(theta2), nh);


				nh = cos(float(h+1) / slice * pi);
				nr = sin(float(h+1) / slice * pi);

				auto v3 = vec3(nr * cos(theta1), nr * sin(theta1), nh);
				auto v4 = vec3(nr * cos(theta2), nr * sin(theta2), nh);

				std::vector<vec3> vert = { v1,v2,v4,v3 };
				glBegin(GL_POLYGON);
					for (auto v : vert) {
						auto proj_vec = mat * vec4(v, 1);
						auto cord = proj_vec.toVec3DivW();
						glTexCoord2f(cord.x,cord.y);
						glVertex3fv(v);
					}
				glEnd();

			}

		}

		glPopMatrix();
	}
};