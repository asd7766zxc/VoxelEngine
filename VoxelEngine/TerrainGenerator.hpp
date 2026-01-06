#pragma once
#include <GL/freeglut.h>
#include <functional>
#include "MathUtility.hpp"
#include "GameObject.hpp"
#include "DrawingUtility.hpp"
#include "Texture.hpp"

//#include "VoxelGenerator.hpp"
class TerrainGenerator:public GameObject {
private:
	std::function<ld(ld, ld)> F; // (x,z) -> y
public:
	std::function<vec3(ld, ld)> dF; // (x,z) -> (x',y',z') normal vector, perpendicular to terrain itselft
	std::function<Color(ld, ld)> colorF;
	std::vector<ld*> points;
	std::vector<ld*> colors;
	shared_ptr<Texture> ground_tex;

	ld width, height;
	int xdim = 0;
	int ydim = 0;
	vec3 pos; // left-up corner
	ld delta;
	TerrainGenerator(auto f, auto df,auto colorf,vec3 _pos,ld w,ld h,ld del = 0.1) :
		F(f), dF(df),colorF(colorf), pos(_pos), width(w), height(h), delta(del) { }
	
	void generate() {
		for (ld y = pos.z; y <= pos.z + height; y += delta) ++ydim;
		for (ld x = pos.x; x <= pos.x + width; x += delta) {
			++xdim;
			for (ld y = pos.z; y <= pos.z + height; y += delta) {
				ld* tmp = new ld[3]{ x, F(x,y) + pos.y, y};
				points.push_back(tmp);
				auto [r, g, b, a] = colorF(x, y);
				ld* col = new ld[3]{r,g,b};
				colors.push_back(col);
			}
		}
	}
	vec3 constrains(vec3 position, vec3 velocity) {
		auto [x, y, z] = position + velocity;
		if (!(pos.z + 1.f <= z && z < pos.z + height - 1.f)) velocity.z *= -1;
		if (!(pos.x + 1.f <= x && x < pos.x + width - 1.f)) velocity.x *= -1;
		ld yy = getY(position.x,position.z);
		//if (y < yy - 0.1f) velocity.y *= -0.5;
		return velocity;
	}
	ld getY(ld x, ld y) {
		ld xx = x - pos.x, yy = y - pos.y;
		x -= pos.x, y -= pos.z;
		x /= delta, y /= delta;
		int i = x, j = y;
		if (getPos(i, j) >= points.size() || getPos(i,j) < 0) return points.back()[1];
		return points[getPos(i, j)][1];
	}
	vec3 getNorm(ld x,ld y) {
		ld xx = x - pos.x, yy = y - pos.y;
		x -= pos.x, y -= pos.z;
		x /= delta, y /= delta;
		int i = x, j = y;
		ld dx = delta - (xx - i * delta);
		ld dy = yy - j * delta;
		if (dx > dy && i < xdim - 1 && j < ydim - 1) {
			auto _v0 = points[getPos(i, j)];
			auto _v1 = points[getPos(i + 1, j)];
			auto _v2 = points[getPos(i, j + 1)];
			vec3 v0 = vec3(_v0);
			vec3 v1 = vec3(_v1);
			vec3 v2 = vec3(_v2);
			v1 = v1 - v0;
			v2 = v2 - v0;
			return (v2 ^ v1);
		}
		else if(i && j) {
			auto _v0 = points[getPos(i, j)];
			auto _v1 = points[getPos(i - 1, j)];
			auto _v2 = points[getPos(i, j - 1)];
			vec3 v0 = vec3(_v0);
			vec3 v1 = vec3(_v1);
			vec3 v2 = vec3(_v2);
			v1 = v1 - v0;
			v2 = v2 - v0;
			return (v2 ^ v1);
		}
		return dF(xx,yy);
	}
	inline int getPos(int i, int j) {
		return i * ydim + j; 
	}
	vec3 getPointNorm(int i, int j) {
		ld x = points[getPos(i, j)][0];
		ld y = points[getPos(i, j)][2];
		auto ret = dF(x, y);
		return ret;
	}
	vec3 getPointTexCoord(int i, int j) {
		return vec3(float(i) / xdim, float(j) / ydim, 0);
	}
	bool isTexturing = false;
	void draw() override {
		applyColorMaterials({ .5,.5,.5,0 }, { .5,.5,.5,0 }, { 0,0,0 }, { 0,0,0,0 }, 0.0);
		if (isTexturing) {
			glEnable(GL_TEXTURE_2D);
			ground_tex->Bind();
			glTexParameteri(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		}

		for (int i = 0; i < xdim; ++i) {
			for (int j = 0; j < ydim; ++j) {
				if (i < xdim - 1 && j < ydim - 1) {
					auto tmp = std::vector<vec3>{ points[getPos(i, j)] ,points[getPos(i + 1, j)] ,points[getPos(i, j + 1)] };
					glNormal3fv((tmp[2] - tmp[0]) ^ (tmp[1] - tmp[0]));
					glBegin(GL_POLYGON);
						//glColor3fv(colors[getPos(i, j)]);
						if(isTexturing)
						glTexCoord2fv(getPointTexCoord(i,j));

						glNormal3fv(getPointNorm(i,j));
						glVertex3fv(points[getPos(i, j)]);

						//glColor3fv(colors[getPos(i+1, j)]);
						if(isTexturing)
						glTexCoord2fv(getPointTexCoord(i+1, j));
						
						glNormal3fv(getPointNorm(i+1, j));
						glVertex3fv(points[getPos(i + 1, j)]);

						//glColor3fv(colors[getPos(i, j+1)]);
						if(isTexturing)
						glTexCoord2fv(getPointTexCoord(i, j+1));
						
						glNormal3fv(getPointNorm(i, j+1));
						glVertex3fv(points[getPos(i, j + 1)]);
					glEnd();
				}

				if (i && j) {
					auto tmp = std::vector<vec3>{ points[getPos(i, j)] ,points[getPos(i - 1, j)] ,points[getPos(i, j - 1)] };
					glNormal3fv((tmp[2] - tmp[0]) ^ (tmp[1] - tmp[0]));
					glBegin(GL_POLYGON);
						//glColor3fv(colors[getPos(i, j)]);
						if(isTexturing)
						glTexCoord2fv(getPointTexCoord(i, j));
						
						glNormal3fv(getPointNorm(i, j));
						glVertex3fv(points[getPos(i, j)]);

						//glColor3fv(colors[getPos(i-1, j)]);
						if(isTexturing)
						glTexCoord2fv(getPointTexCoord(i-1, j));
						
						glNormal3fv(getPointNorm(i-1, j));
						glVertex3fv(points[getPos(i - 1, j)]);
						
						//glColor3fv(colors[getPos(i, j-1)]);
						if(isTexturing)
						glTexCoord2fv(getPointTexCoord(i, j-1));
						
						glNormal3fv(getPointNorm(i, j-1));
						glVertex3fv(points[getPos(i, j - 1)]);
					glEnd();
				}
			}
		}
		
		if (isTexturing) {
			glTexParameteri(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glDisable(GL_TEXTURE_2D);

		}
	}
};