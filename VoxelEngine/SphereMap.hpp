#include "Texture.hpp"
#include "MathUtility.hpp"
#include "Camera.hpp"

class SphereMap : public Texture {
	// Discrete dirction vectors
public:
	shared_ptr<Camera> cam;
	int phi_step = 2;
	int theta_step = 2;
	int dA_h = 10;
	int dA_w = 10;
	SphereMap() {
		cam = make_shared<Camera>(vec3(0, 0, 0), vec3(0, 0, 0));
		base = Texture::tex_count;
		Texture::tex_count += phi_step * theta_step;
		glGenTextures(phi_step * theta_step, &Texture::texes[base]);
	}
	void generate(auto draw_scene) {
		float ang = pi / theta_step;
		cam->make_ortho(1,1,sin(ang)); // dA
		// [-pi/2, pi/2]
		for (int i = 0; i < phi_step; ++i) {
			float phi = (float(i) / (phi_step - 1)) * pi - pi / 2;
			for (int j = 0; j < theta_step; ++j) {
				float theta = (float(j) / (theta_step)) * 2 * pi;
				float x = cos(phi) * cos(theta);
				float y = cos(phi) * sin(theta);
				float z = sin(phi);
				vec3 dir = vec3(x, y, z);
				glViewport(0, 0, dA_w, dA_h);
				cam->lookAt(dir);
				cam->applyMatrix();
				draw_scene();
				int texid = i * theta_step + j;
				glBindTexture(GL_TEXTURE_2D, Texture::texes[base + texid]);
				glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, dA_w, dA_h, 0);
			}
		}

	}
	// (x,y,z) = (texid,x,y)
	vec3 sample(vec3 dir) {
		auto [x, y, z] = dir;
		auto theta = polar_to_cartesian(dir).y;
		auto phi = asin(z);
		int phii = int(round((phi + pi / 2) / pi * (phi_step - 1)));
		int thetai = int(round((theta / (2 * pi)) * theta_step));

	}
	void render() override {
	}
};