#pragma once
#include<array>
#include<vector>
#include <numbers>
template<class T>
struct matrix4 {
	std::vector<std::vector<T>> mt;
	matrix4():mt(4, std::vector<T>(4, 0)) {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j) {
				mt[i][j] = 0;
			}
	}
	std::vector<T>& operator [](int i) {
		return mt[i];
	}
	matrix4 operator + (matrix4 a) {
		matrix4<T> ret();
		for(int i = 0; i < 4;++i)
			for (int j = 0; j < 4; ++j) {
				ret[i, j] = mt[i][j] + a[i][j];
			}
		return ret;
	}
	matrix4 operator - (matrix4 a) {
		matrix4<T> ret();
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j) {
				ret[i, j] = mt[i][j] - a[i][j];
			}
		return ret;
	}

	matrix4 operator * (matrix4 a) {
		matrix4<T> ret;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				for(int k = 0; k < 4;++k){
					ret[i][j] += mt[i][k] * a[k][j];
				}
		return ret;
	}
};
template<class T>
struct vector4 {
	T x, y, z, w;
	vector4(T _x, T _y, T _z, T _w) :x(_x), y(_y), z(_z),w(_w) {};
	vector4(): x(0), y(0), z(0) {};

	vector4 operator + (vector4 a) { return vector4(x + a.x, y + a.y, z + a.z,w + a.w); }
	vector4 operator - (vector4 a) { return vector4(x - a.x, y - a.y, z - a.z,w - a.w); }
	T operator * (vector4 a) { return (x * a.x + y * a.y + z * a.z,w * a.w); }
};

using ld = float;
using vec4 = vector4<ld>;
using mat4 = matrix4<ld>;
template<class T>
struct vector3 {
	T x, y, z;
	vector3(T _x, T _y, T _z) :x(_x), y(_y), z(_z) {};
	vector3(): x(0), y(0), z(0) {};

	vector3 operator + (vector3 a) { return vector3(x + a.x, y + a.y, z + a.z); }
	vector3& operator += (vector3 a) {
		x += a.x, y += a.y, z += a.z;
		return *this;
	}
	vector3 operator * (T a) { return vector3(x*a,y*a,z*a); }
	vector3 operator - (vector3 a) { return vector3(x - a.x, y - a.y, z - a.z); }
	vector3 operator ^ (vector3 a) { return vector3(y * a.z - z * a.y, -(x * a.z - z * a.x), x * a.y - y * a.x); }
	T& operator [](int i) {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		return z;
	}
	T operator * (vector3 a) { return (x * a.x + y * a.y + z * a.z); }
	vector3 operator * (mat4 mat) {
		vector3<T> ret;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 4; ++j)
				if(j != 3) ret[i] += mat[i][j] * this[j];
		}
	}
};
template<class T>
struct _Point2d {
	T x, y;
	_Point2d(T _x, T _y) :x(_x), y(_y) {}
	_Point2d() :x(0), y(0) {}

	_Point2d operator * (T  c) { return _Point2d(x * c, y * c); }
	_Point2d operator / (T  c) { return _Point2d(x / c, y / c); }
	_Point2d operator + (_Point2d a) { return _Point2d(x + a.x, y + a.y); }
	_Point2d operator - (_Point2d a) { return _Point2d(x - a.x, y - a.y); }
	T  operator * (_Point2d a) { return x * a.x + y * a.y; }
	T  operator ^ (_Point2d a) { return x * a.y - y * a.x; }

	bool operator < (_Point2d a) const { return x < a.x || (x == a.x && y < a.y); };
	bool operator== (_Point2d a) const { return x == a.x and y == a.y; };

	friend T ori(_Point2d a, _Point2d b, _Point2d c) { return (b - a) ^ (c - a); }
	friend T abs2(_Point2d a) { return a * a; }
	friend T abs(_Point2d a) { return sqrt(abs2(a)); }
	friend _Point2d unified(_Point2d vec) {
		return vec / abs(vec);
	}
};
using vec2 = _Point2d<ld>;
using vec3 = vector3<ld>;
ld abs2(vec3 a);
ld abs(vec3 a);
ld angle(vec3 a, vec3 b);
vec3 Rx(vec3 v, ld t);
vec3 Ry(vec3 v, ld t);
vec3 Rz(vec3 v, ld t);
vec3 rotate(vec3 v, vec3 axis, vec3 o, ld t);
vec3 uni(vec3 a);
ld radToDegree(ld rad);
using std::numbers::pi;

#define TP(p) (p).x,(p).y,(p).z
