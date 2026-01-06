#pragma once

#include "Utilities.hpp"
#include "DrawingUtility.hpp"
#include <iostream>

class Camera {
public:
	vec3 pos,front;
    vec3 up;
    ld horizontal_sensitivity = 0.003;
    ld vertical_sensitivity = 0.003;
    ld rx = 0.0f;
    ld yx = 0.0f;
    ld rz = 0.0f;

    mat4 view;
    mat4 proj;
    mat4 cproj;
    
    
    void updView() {
        view = mat4::Rz(rz) * mat4::Rx(-rx) * mat4::Ry(yx) * mat4::trans(-pos);
    }
    Camera(vec3 _pos, vec3 _front) : up(0,1,0) {
        pos = _pos;
        front = _front;

        view.makeIdentity();
        proj.makeIdentity();

        updView();
    }
    vec3 getRight() const {
        return (mat4::Ry(-yx) * mat4::Rx(rx)).x_axis();
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

    void make_ortho(int w, int h,float sz = 10) {
        float aspect_ratio = float(h) / float(w);
        float nearp = -0.0f;
        float farp = 100.0;
        float left = -sz;
        float right = sz;
        float bottom = -sz * aspect_ratio;
        float top = sz * aspect_ratio;
        proj.makeZero();
        proj[3 ] = - (right + left) / (right - left);
        proj[7 ] = - (top + bottom) / (top - bottom);
        proj[11] = - (farp + nearp) / (farp - nearp);

        proj[0 ] =  2 / (right - left);
        proj[5 ] =  2 / (top - bottom);
        proj[10] = -2 / (farp - nearp);
        proj[15] = 1;
        //std::cout << proj << '\n';
        return;
    }

    void mouseMove(float dx, float dy) {
        yx += dx * horizontal_sensitivity;
        rx -= dy * vertical_sensitivity;
        if (rx >=  pi / 2) rx = pi / 2 - 0.1f;
        if (rx <= -pi / 2) rx = -pi / 2 + 0.1f;
        
        updView();
        front = -(mat4::Ry(-yx) * mat4::Rx(rx)).z_axis();
	}
    void updatePos(vec3 _pos) {
        pos = _pos;
        updView();
    }
    void lookAt(vec3 _pos) {
        auto nw = pos - _pos;
        auto u = (nw ^ up);
        auto v = (u ^ nw);
        std::vector<vec3> tmp = {uni(u), uni(v), uni(nw)};
        view.makeIdentity();
        mat4 T;
        T.makeIdentity();
        for (int j = 0; j < 3; ++j) {
            T[j * 4 + 3] = -pos[j];
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j)
                view[i * 4 + j] = tmp[i][j];
        }
        view = view * T;
    }
    //https://terathon.com/blog/oblique-clipping.html
    void ObliqueProj(vec3 pos, vec3 norm,bool clipOppo = false) {
        cproj = proj;
        //far plane 在oblique clip space 會被忽略 但太大會造成數值誤差
        //所以直接開 1.0f 
        cproj[10] = (1.0f + 0.01f) / (0.01f - 1.0f);
        cproj[11] = (2 * 1.0f * 0.01f) / (0.01f - 1.0f);

        vec3 cpos = (view * vec4(pos, 1)).toVec3();
        vec3 cnorm = (view * vec4(norm, 0)).toVec3();
		vec4 plane = vec4(cnorm.x, cnorm.y, cnorm.z, -(cnorm * cpos));
        vec4 q = cproj.inverse() * vec4(
            plane.x < 0.0f ? 1.0f : -1.0f,
            plane.y < 0.0f ? 1.0f : -1.0f,
            1.0f, 1.0f );
		vec4 c = plane * (2.0f / -(plane * q));

        if (clipOppo) c = c * (-1.0f);

        // 注意 replace 後 inverse 會變。
        // 所以直接開一個新的
        cproj.mt[8] = c.x;
        cproj.mt[9] = c.y;
        cproj.mt[10] = c.z + 1.0f;
        cproj.mt[11] = c.w;
    }
    mat4 Matrix() const {
        return (proj * view);
    }

    //clip space projection
    mat4 cMatrix() const {
        return (cproj * view);
    }

    void useProj() const {
        glLoadMatrixf(proj.transposed().mt);
	}
    void useView() const {
        glLoadMatrixf(view.transposed().mt);
	}
    void useCProj() const {
        glLoadMatrixf(cproj.transposed().mt);
	}
    
    void applyMatrix() const {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(proj.transposed().mt);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(view.transposed().mt);
    }
    void applyCMatrix() const {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(cproj.transposed().mt);
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(view.transposed().mt);
	}

    void useMatrix() const {
        glLoadMatrixf(Matrix().transposed().mt);
    }
    
    void useCMatrix() {
        glLoadMatrixf(cMatrix().transposed().mt);
    }
};