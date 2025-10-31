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
		scale = linkto->scale; //�������
	}
	float render_offset = 10.0f;
	// ���F�קK�ǰe�ɳy�����e�b�y�W(�}��) �����D
	// �������ܩ� forward ��V���e��
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
		float dis = 5.0f;

		//dis ���O�S�t�� ���Y���|�ݰ_�ӹ��첾
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

		// �ѩ�Ҧ� '�y��' ���|�Q���o�� Portal (Oblique �b��ɬO�� view matrix) 
		// �ҥH�o���t�@�Ӫ������� (�|�Q���o�Ӫ��W) �h�� Oblique Clip
		pcam.ObliqueProj(_pos, linkto.forward());

		glLoadMatrixf(cam.Matrix().transposed().mt);

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

		glLoadMatrixf(pcam.cMatrix().transposed().mt);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_LEQUAL, 1, 0xFF); //if (ref & mask) <= (stencil & mask)

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
	}
	void camEndDraw() {
		//�����o�� portal �b depth buffer �A�e�@�� (portal���i��|�Q�צ�)
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_TRUE);

		draw();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
};