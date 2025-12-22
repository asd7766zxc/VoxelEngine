#pragma once
#include "GameObject.hpp"
#include "Camera.hpp"

class Portal : public GameObject {
public:
	Portal* linkto;
	Portal() {
		linkto = nullptr;
	}
	Portal(Portal* _linkto) {
		linkto = _linkto;
		scale = linkto->scale; //防止形變
	}

	float render_offset = 0.0f;
	// 為了避免傳送時造成門畫在臉上(破圖) 的問題
	// 把門的顯示往 forward 方向往前移
	void draw() override {
		glPushMatrix();
			glMultMatrixf(localToWorld().transposed().mt);
			glMultMatrixf(matrix4::trans(vec3(0,0, render_offset)).transposed().mt);
			draw_unirectangle();
		glPopMatrix();
	}

	void warp(Camera* camera) {
		if (linkto == nullptr) return;
		vec3 v = camera->pos - pos;
		float dis = 3.0f;

		//dis 其實是沒差的 鏡頭不會看起來像位移
		if (uni(forward()) * v < dis && abs(v) < 10.0f) {
			camera->pos = (linkto->localToWorld() * worldToLocal() * vec4(camera->pos, 1)).toVec3();
		}
	}
	void camDraw(const Camera& cam) {
		camDraw(cam,*linkto);
	}

	void camDraw(const Camera& cam,const Portal& linkto) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		Camera pcam = cam;
		pcam.view = cam.view * (localToWorld() * linkto.worldToLocal());
		
		vec3 norm = linkto.forward();
		vec3 _pos = linkto.pos;

		// 由於所有 '座標' 都會被轉到這個 Portal (Oblique 在算時是用 view matrix) 
		// 所以得拿另一個門的平面 (會被轉到這個門上) 去做 Oblique Clip
		pcam.ObliqueProj(_pos, linkto.forward());

		cam.applyMatrix();
		//glLoadMatrixf(cam.Matrix().transposed().mt);

		//disable draw to color&depth buffer
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);

		// draw to stencil buffer
		glStencilMask(0xff);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);

		//always failed
		glStencilFunc(GL_NEVER, 0, 0); 
		glClear(GL_STENCIL_BUFFER_BIT);

		draw();

		pcam.applyCMatrix();

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_LEQUAL, 1, 0xFF); //if (ref & mask) <= (stencil & mask)

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
	}
	void camEndDraw() {
		//收尾得把 portal 在 depth buffer 再畫一次 (portal有可能會被擋住)
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_TRUE);

		draw();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
};