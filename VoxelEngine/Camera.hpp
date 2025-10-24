#pragma once
#include "MathUtility.hpp"
#include "DrawingUtility.hpp"
class Camera {
public:
	vec3 pos,front;
    vec3 up;
    ld horizontal_sensitivity = 0.003;
    ld vertical_sensitivity = 0.003;
    ld rx = 0.0f;
    ld yx = 0.0f;

    mat4 view;

    Camera(vec3 _pos, vec3 _front):up(0,1,0) {
        pos = _pos;
        front = _front;
        view.makeIdentity();
        view = mat4::Rx(-rx) * mat4::Ry(yx) * mat4::trans(-pos);
    }

    void mouseMove(float dx, float dy) {
        yx += dx * horizontal_sensitivity;
        rx -= dy * vertical_sensitivity;
        if (rx >= pi / 2) rx = pi / 2 - 0.1f;
        if (rx <= -pi / 2) rx = -pi / 2 + 0.1f;
        
		view = mat4::Rx(-rx) * mat4::Ry(yx) * mat4::trans(-pos);
        front = (-view.inverse().z_axis() + pos);
	}

    void LookAt() {
        glLoadMatrixf(view.transposed().mt);
    }
};