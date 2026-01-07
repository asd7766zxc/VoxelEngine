#pragma once
#include "Texture.hpp"

class ShootingStarTexture : public Texture {
public:
    float width, height;
    float scale = 1.0f;
    BYTE data[512 * 512 * 4];
    struct spark {
        vec3 sped;
        vec3 pos;
        void draw(float scale) {
            auto halo = [&](float r, Color color) {
                int slice = 100;
                glPushMatrix();
                glTranslatef(.5, .5, 0);
                for (int i = 0; i < slice; ++i) {
                    float theta = (float(i) / slice) * 2 * pi;
                    float theta2 = (float(i + 1) / slice) * 2 * pi;
                    glBegin(GL_POLYGON);
                    glColor4f(0, 0, 0, 0);
                    glVertex2f(r * cos(theta), r * sin(theta));
                    glColor4f(0, 0, 0, 0);
                    glVertex2f(r * cos(theta2), r * sin(theta2));
                    glColor4fv(color);
                    glVertex2f(0, 0);

                    glEnd();
                }
                glPopMatrix();
            };
            glPushMatrix();
            glTranslatef(pos.x, pos.y, 0);
            halo(0.01 * scale, Color(1, 1, 0,0.5 * abs(vec3(1,1,0) - pos) / sqrt(2)));
            glPopMatrix();
            pos += sped * scale;
            if (pos.x > 1 || pos.y > 1) pos = vec3(0, 0, 0);
        }
    };
    std::vector<spark> spks;
	ShootingStarTexture(float width,float height,float scale):
        width(width),height(height),scale(scale){
        for (int i = 0; i < 10; ++i) {
            vec3 sped;
            sped = vec3(0.001 + random_double() / 1000, 0.001 + random_double() / 1000 ,0);
            spks.push_back(spark{ sped });
        }
		allocate(1);
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
    void drawHalo(float r,Color color) {
        int slice = 100;
        glPushMatrix();
        glTranslatef(.5, .5, 0);
        for (int i = 0; i < slice; ++i) {
            float theta = (float(i) / slice) * 2 * pi;
            float theta2 = (float(i + 1) / slice) * 2 * pi;
            glBegin(GL_POLYGON);
            glColor4f(0, 0, 0, 0);
            glVertex2f(r * cos(theta),r * sin(theta));
            glColor4f(0, 0, 0, 0);
            glVertex2f(r * cos(theta2), r * sin(theta2));
            glColor4fv(color);
            glVertex2f(0,0);

            glEnd();
        }
        glPopMatrix();
    }
    float ticks = 0.0;
    
	void render() override {
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
        glViewport(0, 0, width, height);

        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluOrtho2D(0, width / height, 0, 1);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 0.1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (ticks >= pi) ticks = 0;
        ticks += 0.05;
        
        glPushMatrix();

        //drawHalo(0.08 * scale, Color(1,1,1,0.4 * pow(sin(ticks),0.05)));
        //drawHalo(0.02 * scale, Color(1, 1, 0, 0.4));
        drawHalo(0.01 * scale, Color(1,1,0,1));
        glBegin(GL_POLYGON);
        glColor4fv(Color(1, 1, 1, 1));
        glVertex2f(.5, .5);
        glVertex2f(.5, .5 - 0.002 * scale);
        glColor4fv(Color(0, 0, 0, 0));
        glVertex2f(1, 1 - 0.002 * scale);
        glVertex2f(1, 1);
        glEnd();
        for (auto &spk : spks) {
            spk.draw(scale);
        }
        
        glPopMatrix();
        glEnable(GL_TEXTURE_2D);
        glReadBuffer(GL_BACK);
        glBindTexture(GL_TEXTURE_2D, getTexID(0));
        
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        for(int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
				int ind = (j * width + i) * 4;
                if(data[ind] == data[ind + 1] && data[ind] == data[ind + 2] && data[ind + 2] == 0) {
                    data[ind] = 0;
                    data[ind + 1] = 0;
                    data[ind + 2] = 0;
                    data[ind + 3] = 0;
                }
                else if(data[ind]) {
					data[ind + 3] = data[ind + 1];
                }
            }
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);

	}
};
