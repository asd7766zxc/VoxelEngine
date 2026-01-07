#pragma once
#include "GameObject.hpp"
#include "Camera.hpp"
#include "EnderTexture.hpp"
#include "ShootingStarTexture.hpp"
#include "billboard.hpp"

class Skybox : public GameObject {
public:
    shared_ptr<EnderTexture> end_tex;
    shared_ptr<ShootingStarTexture> shoot_tex;
    std::vector<shared_ptr<billboard>> bills;
    Skybox() {
        end_tex = make_shared<EnderTexture>(512,512,1.0f,0.1f,Color(0x00FFFF),200,Color(0,0,0,1));
        shoot_tex = make_shared<ShootingStarTexture>(512, 512, 1.0f);
        for (int i = 0; i < 10; ++i) {
            auto bill = make_shared<billboard>(shoot_tex);
            bill->size = 40.0f + 40 * random_double();
            bill->angle = 10.0f;
            bill->pos = vec3(25 + random_double() * 40, 25 + random_double() * 40,random_double() * 40);
			bills.push_back(bill);
        }
      
    }
	void render(Camera* cam) {
        glDisable(GL_LIGHTING);
		glPushMatrix();

        //Background

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glLoadMatrixf(cam->proj.transposed().mt);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        glLoadMatrixf(cam->view.transposed().mt);
		glMultMatrixf(mat4::trans(cam->pos).transposed().mt);


        std::vector<vec3> tmp{ {0,0,0},{1,0,0},{1,1,0},{0,1,0} };
        for (int i = 0; i < 6; ++i) {
            glPushMatrix();
            glScalef(130,130,130);
            glBegin(GL_POLYGON);
            for (int ii = 3; ii >= 0; --ii) {
                glTexCoord2f(tmp[ii].x,tmp[ii].y);
                glVertex3fv(vec3(PrimitiveShape::cube[PrimitiveShape::face[i][ii]])  
                    - vec3(0.5, 0.5, 0.5));

            }
            glEnd();
            glPopMatrix();
        }

        glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.2f);
        for (auto bill : bills) {
			auto w = uni(bill->pos);
			auto v = vec3(0, 1, 0);
			auto u = uni(v ^ w);
			v = w ^ u;
			v = rotate(v, uni(bill->pos), -DegreeToRad(35));
			bill->pos = rotate(bill->pos, uni(v), 0.001f);
            bill->norm = bill->pos;
			bill->draw(cam);
        }
		glDisable(GL_ALPHA_TEST);
		glPopMatrix();

	}
};