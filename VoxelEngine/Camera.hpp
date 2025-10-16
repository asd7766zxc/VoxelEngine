#pragma once
#include "MathUtility.hpp"
class Camera {
public:
	vec3 pos,front;
    vec3 up;
    ld horizontal_sensitivity = 0.003;
    ld vertical_sensitivity = 0.003;
    ld yaw = 0.0f;
    ld pitch = 0.0f;
    Camera(vec3 _pos, vec3 _front):up(0,1,0) {
        pos = _pos;
        front = _front;
    }

    void mouseMove(float dx, float dy) {
        pitch += dx * horizontal_sensitivity;
        yaw += dy * vertical_sensitivity;

        if (yaw >= pi / 2) yaw = pi / 2 - 0.1f;
        if (yaw <= -pi / 2) yaw = -pi / 2 + 0.1f;
        
        vec3 w = vec3(cos(pitch) * cos(yaw), cos(pitch) * sin(yaw),sin(pitch));
        front = pos + w;
        //right
        vec3 u = uni(w ^ vec3(0, 1, 0));
        up = uni(u ^ w);
	}
    void LookAt() {
        gluLookAt(TP(pos),TP(front),TP(up));
    }
    /*gluLookAt(camera->pos.x, camera->pos.y, camera->pos.z, \
        camera->front.x, camera->front.y, camera->front.z, \
        camera->up.x, camera->up.y, camera->up.z);*/

};