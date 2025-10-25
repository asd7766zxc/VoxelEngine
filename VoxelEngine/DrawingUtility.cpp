#pragma once
#include "DrawingUtility.hpp"
#include "MathUtility.hpp"

void draw_unicube() {
    for (int i = 0; i < 6; i++) {
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

void drawLink(vec3 a, vec3 b) {
    vec3 axi = b - a; // ��Ŷ��ন b - a �O z-axis

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