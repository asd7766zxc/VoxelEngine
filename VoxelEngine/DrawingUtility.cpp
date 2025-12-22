#pragma once
#include "DrawingUtility.hpp"

void draw_unicube() {
    for (int i = 0; i < 6; i++) {
		glNormal3fv(PrimitiveShape::norms[i]);
        glBegin(GL_POLYGON);  /* Draw the face */
            glVertex3fv(PrimitiveShape::cube[PrimitiveShape::face[i][0]]);
            glVertex3fv(PrimitiveShape::cube[PrimitiveShape::face[i][1]]);
            glVertex3fv(PrimitiveShape::cube[PrimitiveShape::face[i][2]]);
            glVertex3fv(PrimitiveShape::cube[PrimitiveShape::face[i][3]]);
        glEnd();
    }
}

//draw a cylinder span toward +z-aixs (base sits on x-y plane)
GLUquadricObj* unicylind;
void draw_unicylind() {
    if (unicylind == nullptr) {
        unicylind = gluNewQuadric();
        gluQuadricDrawStyle(unicylind, GLU_FILL);
        gluQuadricNormals(unicylind, GLU_SMOOTH);
    }
    gluCylinder(unicylind,1,1,1,12,3);
}

GLUquadricObj* unisphere;
void draw_unisphere() {
    if (unisphere == NULL) {
        unisphere = gluNewQuadric();
        gluQuadricDrawStyle(unisphere, GLU_FILL);
        gluQuadricNormals(unisphere, GLU_SMOOTH);
    }
    gluSphere(unisphere,1.0,12,12);
}
void draw_unirectangle() {
    glBegin(GL_POLYGON);
        glVertex3f(-1, -1, 0);
        glVertex3f(1, -1, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(-1, 1, 0);
    glEnd();
}

void applyMaterials(Color ambient, Color diffuse, Color specular,Color emission, float shininess) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void applyColorMaterials(Color ambient,Color diffuse, Color specular, Color emission, float shininess) {
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glColor4fv((diffuse));
    glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
    glColor4fv((specular));
	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
    glColor4fv((emission));
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
    glColor4fv((ambient));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}


void drawAxis(float len) {
    std::vector<Color> colors = { {1,0,0},{0,1,0},{0,0,1} };
    std::vector<vec3> axiss;

    for (int i = 0; i < 3; i++) {
        glColor3f(TC(colors[i % 3]));
        auto u = vec3(TC(colors[i])) * len;
        axiss.push_back(u);
    }
    for (int i = 0; i < axiss.size(); ++i) {
        glColor3f(TC(colors[i % 3]));
        glPushMatrix();
            alignZTo(axiss[i]);
            glScalef(.2, .2, abs(axiss[i]));
            draw_unicylind();
        glPopMatrix();
    }
}

void alignedWithZ(vec3 axis) {
    vec3 zaxi(0, 0, 1);
    vec3 nox(0, axis.y, axis.z);
    ld ofa = angle(nox, zaxi);
    axis = Rx(axis, ofa);
    ld ofa1 = -angle(axis, zaxi);
    glRotatef(radToDegree(ofa1), 0, 1, 0);
    glRotatef(radToDegree(ofa), 1, 0, 0);
}

void alignZTo(vec3 axis) {
    vec3 zaxi(0, 0, 1);
    vec3 norm = uni(axis ^ zaxi);
    ld ang = -angle(axis, zaxi);
    if (axis.y < 1e-4 && axis.x < 1e-4) {
        norm = uni(vec3(0, axis.z,0));
        if (axis.z < 0) ang = pi;
    }
    glRotatef(radToDegree(ang), TP(norm));
}

void drawLine(vec3 A,vec3 B) {
    glPushMatrix();
        glTranslatef(TP(A));
        alignZTo(uni(B - A));
        glScalef(.2, .2, abs(B - A));
        draw_unicylind();
    glPopMatrix();
}

Color hsv_to_rgb(int h, float s, float v) {
    int hi = h / 60;
    float f, p, q, t;
    f = h / 60.0 - hi;
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
    switch (hi) {
    case 0:
        return Color(v, t, p);
    case 1:
        return Color(q, v, p);
    case 2:
        return Color(p, v, t);
    case 3:
        return Color(p, q, v);
    case 4:
        return Color(t, p, v);
    case 5:
        return Color(v, p, q);
    }
    return Color{};
}
Color GetColorFrom(float angle) {
    float sat = 0.5f;
    int h = (angle / pi / 2.0f) * 360;
    int hue = h % 360;
    return hsv_to_rgb(hue, sat, 1.0f);
}
float RadToDegree(float rad) {
    return rad * 180.0f / pi;
}
float DegreeToRad(float deg) {
    return deg * pi / 180.0f;
}
//std::pair<ld,vec3> alignZTo(vec3 axis) {
//    vec3 zaxi(0, 0, 1);
//    vec3 norm = uni(axis ^ zaxi);
//    ld ang = -angle(axis, zaxi);
//    if (axis.y < 1e-4 && axis.x < 1e-4) {
//        norm = uni(vec3(0, axis.z, 0));
//        if (axis.z < 0) ang = pi;
//    }
//    return { ang,norm };
//}