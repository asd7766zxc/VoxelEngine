#include "GameObject.hpp"
#include "Camera.hpp"
class Portal : public GameObject {
public:
	void draw() override {
		glPushMatrix();
			glMultMatrixf(localToWorld().transposed().mt);
			rectangle();
		glPopMatrix();
	}
	void rectangle() {
		glColor3f(1, 1, 1);
		glBegin(GL_POLYGON);  
			glVertex3f(-1, -1, 0);
			glVertex3f(1, -1, 0);
			glVertex3f(1, 1, 0);
			glVertex3f(-1, 1, 0);
		glEnd();
	}
	void camDraw(const Camera& cam,const Portal& linkto) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		Camera pcam = cam;
		pcam.view = cam.view * (localToWorld() * linkto.worldToLocal());

		//far plane �boblique clip space �|�Q���� ���Ӥj�|�y���ƭȻ~�t
		//�ҥH�����} 1.0f 
		pcam.proj[10] = (1.0f + 0.01f) / (0.01f - 1.0f);
		pcam.proj[11] = (2 * 1.0f * 0.01f) / (0.01f - 1.0f);


		vec3 norm = linkto.forward();
		vec3 _pos = linkto.pos;

		// �ѩ�Ҧ� '�y��' ���|�Q���o�� Portal (Oblique �b��ɬO�� view matrix) 
		// �ҥH�o���t�@�Ӫ������� (�|�Q���o�Ӫ��W) �h�� Oblique
		pcam.ObliqueProj(_pos, linkto.forward());

		glLoadMatrixf(cam.Matrix().transposed().mt);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

		glPushMatrix();
			glMultMatrixf(localToWorld().transposed().mt);
			rectangle();
		glPopMatrix();

		glLoadMatrixf(pcam.cMatrix().transposed().mt);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_LEQUAL, 1, 0xFF); //if (ref & mask) >= (stencil & mask)

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
	}
	void camEndDraw() {
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		glClear(GL_DEPTH_BUFFER_BIT);

		glPushMatrix();
			glMultMatrixf(localToWorld().transposed().mt);
			rectangle();
		glPopMatrix();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
};