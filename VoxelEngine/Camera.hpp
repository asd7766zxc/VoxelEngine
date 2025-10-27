#pragma once
#include "MathUtility.hpp"
#include "DrawingUtility.hpp"
#include "Voxel.hpp"
#include <iostream>
class Camera {
public:
	vec3 pos,front;
    vec3 up;
    ld horizontal_sensitivity = 0.003;
    ld vertical_sensitivity = 0.003;
    ld rx = 0.0f;
    ld yx = 0.0f;

    mat4 view;
    mat4 proj;
    mat4 cproj;

    Camera(vec3 _pos, vec3 _front) : up(0,1,0) {
        pos = _pos;
        front = _front;

        view.makeIdentity();
        proj.makeIdentity();

        view = mat4::Rx(-rx) * mat4::Ry(yx) * mat4::trans(-pos);
    }
    void resize(int w,int h,float nearp,float farp,float fov) {

        const float e = 1.0f / std::tan(fov * pi / 360.f);
        const float a = float(h) / float(w);
        const float d = nearp - farp;

		proj.makeZero();

        proj[0] = e * a;
        proj[5] = e;
        proj[10] = (farp + nearp) / d;
        proj[11] =  (2 * farp * nearp) / d;
		proj[14] = -1.0f;

    }

    void mouseMove(float dx, float dy) {
        yx += dx * horizontal_sensitivity;
        rx -= dy * vertical_sensitivity;
        if (rx >=  pi / 2) rx = pi / 2 - 0.1f;
        if (rx <= -pi / 2) rx = -pi / 2 + 0.1f;
        
		view = mat4::Rx(-rx) * mat4::Ry(yx) * mat4::trans(-pos);
        front = (-view.inverse().z_axis());
	}
    void updatePos(vec3 _pos) {
        pos = _pos;
        view = mat4::Rx(-rx) * mat4::Ry(yx) * mat4::trans(-pos);
    }
    //https://terathon.com/blog/oblique-clipping.html
    void ObliqueProj(vec3 pos, vec3 norm) {
        vec3 cpos = (view * vec4(pos, 1)).toVec3();
        vec3 cnorm = (view * vec4(norm, 0)).toVec3();
		vec4 plane = vec4(cnorm.x, cnorm.y, cnorm.z, -(cnorm * cpos));
        vec4 q = proj.inverse() * vec4(
            plane.x < 0.0f ? 1.0f : -1.0f,
            plane.y < 0.0f ? 1.0f : -1.0f,
            1.0f, 1.0f );
		vec4 c = plane * (2.0f / -(plane * q));
        cproj = proj;

        cproj.mt[8] = c.x;
        cproj.mt[9] = c.y;
        cproj.mt[10] = c.z + 1.0f;
        cproj.mt[11] = c.w;
    }
    // 注意 replace 後 inverse 會變。
    mat4 Matrix() const {
        return (proj * view);
    }
    mat4 cMatrix() const {
        return (cproj * view);
    }
    void LookAt() {
        //glLoadMatrixf(Matrix().transposed().mt);
        glLoadMatrixf(view.transposed().mt);
    }
};