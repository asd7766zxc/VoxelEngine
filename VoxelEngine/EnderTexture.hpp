#pragma once
#include <GL/glut.h>
#include "MathUtility.hpp"
#include "DrawingUtility.hpp"
#include "Texture.hpp"

const Color purple = Color(207 / 255.0, 159 / 255.0, 255 / 255.0, 1.0);
struct Trail {
    vec3 pos;
    vec3 sped;
    vec3 scale;
    float ang = 0;
    float alpha = 1;
    Color color;
    Trail(Color color) :color(color), pos(0,0,0),scale(1,1,1) {

    }
    void drawRec() {
        glBegin(GL_POLYGON);
        glVertex2f(0, 0);
        glVertex2f(0.2, 0);
        glVertex2f(0.2, 0.2);
        glVertex2f(0, 0.2);
        glEnd();
    }
    void draw() {
        int n = 5;
        glPushMatrix();
        toWorld();
        for (int i = 0; i < n; ++i) {
            glPushMatrix();
            glTranslatef(i * 0.2, 0, 0);
            Color pur = color;
            pur.a = float(i) / (n - 1);
            pur.a = pur.a * pur.a * pur.a * alpha;
            glColor4fv(pur);
            drawRec();
            glPopMatrix();
        }
        glPopMatrix();
    }
    void toWorld() {
        glTranslatef(pos.x,pos.y,0);
        glRotatef(ang * 180 / pi, 0, 0, 1);
        glScalef(scale.x,scale.y,scale.z);
    }
    void upd(float width,float height) {
        pos += dt * sped;
        if (pos.x > width) pos.x = 0;
        if (pos.x < 0) pos.x = width;
        if (pos.y > height) pos.y = 0;
        if (pos.y < 0) pos.y = height;
    }
};
class EnderTexture : public Texture {
public:
    float width = 1;
    float height = 1;
    float scale = 1;
    int limit = 1000;
    float maxmum = 1;
    Color color = purple;
    std::vector<Trail> trails;
    Color bg_color;
    EnderTexture(float width,float height,float scale = 1.0f,float maxmum = 1.0f,Color color = purple,int limit = 1000,Color bg_color = {0,0,0,1}) :
        width(width),height(height),scale(scale),maxmum(maxmum),color(color),limit(limit),bg_color(bg_color) {
        allocate(1);
        Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        generate();
    }
    void generate() {
        for (int i = 0; i < limit; ++i) {
            Trail trail(color);
            trail.ang = random_double() * 2 * pi;
            trail.alpha = random_double() * random_double();
            float x = random_double();
            float y = std::pow(x, 20);
            y = std::max(y, 0.1f);
            y = std::min(y, maxmum);
            trail.scale = vec3(1,1,1) * y * 0.2 * scale;
            trail.pos = vec3(random_double() * width / height,random_double(),0);
            trail.sped = vec3(cos(trail.ang) * width / height, sin(trail.ang), 0) * x  * 0.2 * scale;
            trails.push_back(trail);
        }
    }
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
        gluOrtho2D(0, width/height, 0, 1);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(T4C(bg_color));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        for (auto& trail : trails) {
            trail.upd(width/height,1.0);
            trail.draw();
        }
        glDisable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glReadBuffer(GL_BACK);
        glBindTexture(GL_TEXTURE_2D, getTexID(0));
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	}
};