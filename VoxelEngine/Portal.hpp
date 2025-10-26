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
			glVertex3f(0, 0, 0);
			glVertex3f(10, 0, 0);
			glVertex3f(10, 10, 0);
			glVertex3f(0, 10, 0);
		glEnd();
	}
	void camDraw(const Camera& cam,const Portal& linkto) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		Camera pcam = cam;
		vec3 norm = forward();
		pcam.view = cam.view * (localToWorld() * linkto.worldToLocal());
		pcam.ObliqueProj(pos - norm,norm);

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

		glLoadMatrixf(pcam.Matrix().transposed().mt);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_LEQUAL, 1, 0xFF); //if (ref & mask) >= (stencil & mask)

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
	}
	void camEndDraw() {
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_TRUE);

		glClear(GL_DEPTH_BUFFER_BIT);

		glPushMatrix();
			glMultMatrixf(worldToLocal().transposed().mt);
			rectangle();
		glPopMatrix();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
};