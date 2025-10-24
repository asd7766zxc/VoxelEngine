#pragma once
#include<array>
#include<vector>
#include <numbers>
#include <cmath>

template<class T>
struct vector3 {
	T x, y, z;
	vector3(T _x, T _y, T _z) :x(_x), y(_y), z(_z) {};
	vector3(T* arr) :x(arr[0]), y(arr[1]), z(arr[2]) {};
	vector3() : x(0), y(0), z(0) {};

	vector3 operator + (vector3 a) { return vector3(x + a.x, y + a.y, z + a.z); }
	inline void operator += (vector3 a) {
		x += a.x, y += a.y, z += a.z;
	}
	vector3 operator * (T a) { return vector3(x * a, y * a, z * a); }
	inline void operator *= (T c) {
		x *= c, y *= c, z *= c;
	}
	vector3 operator - (vector3 a) { return vector3(x - a.x, y - a.y, z - a.z); }
	inline void operator -= (vector3 a) {
		x -= a.x, y -= a.y, z -= a.z;
	}
	vector3 operator ^ (vector3 a) { return vector3(y * a.z - z * a.y, -(x * a.z - z * a.x), x * a.y - y * a.x); }
	T& operator [](int i) {
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		return z;
	}
	T operator * (vector3 a) { return (x * a.x + y * a.y + z * a.z); }

	inline vector3 operator-() const {
		return vector3(-x, -y, -z);
	}
};

using ld = float;
using vec3 = vector3<ld>;


class matrix4 {
public:
	//row-major, need to be transposed for OpenGL
	float mt[16];
	matrix4() { 
		makeZero();
	}
	float& operator [](int i) {
		return mt[i];
	}
	matrix4 operator + (matrix4 a) {
		matrix4 ret;
		for (int i = 0; i < 16; ++i) ret[i] = mt[i] + a[i];
		return ret;
	}
	matrix4 operator - (matrix4 a) {
		matrix4 ret;
		for (int i = 0; i < 16; ++i) ret[i] = mt[i] - a[i];
		return ret;
	}

	void operator /= (float c) {
		for (int i = 0; i < 16; ++i) mt[i] /= c;
	}
	void operator *= (float c) {
		for (int i = 0; i < 16; ++i) mt[i] *= c;
	}
	void operator += (matrix4 a) {
		for (int i = 0; i < 16; ++i) mt[i] += a[i];
	}

	matrix4 operator * (matrix4 a) {
		matrix4 ret;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				for (int k = 0; k < 4; ++k) {
					ret[i * 4 + j] += mt[i * 4 + k] * a[k * 4 + j];
				}
		return ret;
	}


	inline void makeRX(float a) {
		mt[0]  = 1.0f;	 mt[1]	= 0.0f;        mt[2]  = 0.0f;			mt[3]  = 0.0f;
		mt[4]  = 0.0f;	 mt[5]	= std::cos(a); mt[6]  = -std::sin(a);	mt[7]  = 0.0f;
		mt[8]  = 0.0f;	 mt[9]	= std::sin(a); mt[10] =  std::cos(a);	mt[11] = 0.0f;
		mt[12] = 0.0f;	 mt[13] = 0.0f;        mt[14] = 0.0f;			mt[15] = 1.0f;
	}
	inline void makeRY(float a) {
		mt[0]  =  std::cos(a); mt[1]  = 0.0f; mt[2]  = std::sin(a); mt[3] = 0.0f;
		mt[4]  = 0.0f;         mt[5]  = 1.0f; mt[6]  = 0.0f;        mt[7] = 0.0f;
		mt[8]  = -std::sin(a); mt[9]  = 0.0f; mt[10] = std::cos(a); mt[11] = 0.0f;
		mt[12] = 0.0f;         mt[13] = 0.0f; mt[14] = 0.0f;        mt[15] = 1.0f;
	}
	inline void makeRZ(float a) {
		mt[0]  = std::cos(a); mt[1]	 = -std::sin(a); mt[2]	= 0.0f; mt[3]  = 0.0f;
		mt[4]  = std::sin(a); mt[5]	 =  std::cos(a);  mt[6]	= 0.0f; mt[7]  = 0.0f;
		mt[8]  = 0.0f;        mt[9]	 = 0.0f;         mt[10] = 1.0f; mt[11] = 0.0f;
		mt[12] = 0.0f;        mt[13] = 0.0f;         mt[14] = 0.0f; mt[15] = 1.0f;
	}
	inline void makeTrans(vec3 t) {
		mt[0] = 1.0f; mt[1] = 0.0f; mt[2] = 0.0f; mt[3] = t.x;
		mt[4] = 0.0f; mt[5] = 1.0f; mt[6] = 0.0f; mt[7] = t.y;
		mt[8] = 0.0f; mt[9] = 0.0f; mt[10] = 1.0f; mt[11] = t.z;
		mt[12] = 0.0f; mt[13] = 0.0f; mt[14] = 0.0f; mt[15] = 1.0f;
	}
	inline void makeScale(vec3 s) {
		mt[0] = s.x;  mt[1] = 0.0f; mt[2] = 0.0f; mt[3] = 0.0f;
		mt[4] = 0.0f; mt[5] = s.y;  mt[6] = 0.0f; mt[7] = 0.0f;
		mt[8] = 0.0f; mt[9] = 0.0f; mt[10] = s.z;  mt[11] = 0.0f;
		mt[12] = 0.0f; mt[13] = 0.0f; mt[14] = 0.0f; mt[15] = 1.0f;
	}

	void makeIdentity() {
		makeZero();
		for (int i = 0; i < 4; ++i) mt[i * 4 + i] = 1;
	}
	void makeZero() {
		std::fill(mt, mt + 16, 0);
	}

	inline static matrix4 Rx(float a) { matrix4 m; m.makeRX(a);	     return m; }
	inline static matrix4 Ry(float a) { matrix4 m; m.makeRY(a);		 return m; }
	inline static matrix4 Rz(float a) { matrix4 m; m.makeRZ(a);		 return m; }
	inline static matrix4 trans(vec3 t) { matrix4 m; m.makeTrans(t); return m; }
	inline static matrix4 identity()  { matrix4 m; m.makeIdentity(); return m; }

	//Transpose
	inline matrix4 transposed() const {
		matrix4 ret;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				ret[j * 4 + i] = mt[i * 4 + j];
		return ret;
	}

	//Inverse
	matrix4 inverse() const {
		matrix4 ret;

		ret.mt[0] = mt[5]  * mt[10] * mt[15] -
					mt[5]  * mt[11]	* mt[14] -
					mt[9]  * mt[6]	* mt[15] +
					mt[9]  * mt[7]	* mt[14] +
					mt[13] * mt[6]	* mt[11] -
					mt[13] * mt[7]	* mt[10];

		ret.mt[4] = -mt[4]	* mt[10] * mt[15] +
					 mt[4]	* mt[11] * mt[14] +
					 mt[8]	* mt[6]	 * mt[15] -
					 mt[8]	* mt[7]	 * mt[14] -
					 mt[12] * mt[6]	 * mt[11] +
					 mt[12] * mt[7]	 * mt[10];

		ret.mt[8] = mt[4]  * mt[9]  * mt[15] -
					mt[4]  * mt[11] * mt[13] -
					mt[8]  * mt[5]  * mt[15] +
					mt[8]  * mt[7]  * mt[13] +
					mt[12] * mt[5]  * mt[11] -
					mt[12] * mt[7]  * mt[9];

		ret.mt[12] = -mt[4]  * mt[9]  * mt[14] +
					  mt[4]  * mt[10] * mt[13] +
					  mt[8]  * mt[5]  * mt[14] -
					  mt[8]  * mt[6]  * mt[13] -
					  mt[12] * mt[5]  * mt[10] +
					  mt[12] * mt[6]  * mt[9];

		ret.mt[1] = -mt[1]  * mt[10] * mt[15] +
					 mt[1]  * mt[11] * mt[14] +
					 mt[9]  * mt[2]  * mt[15] -
					 mt[9]  * mt[3]  * mt[14] -
					 mt[13] * mt[2]  * mt[11] +
					 mt[13] * mt[3]  * mt[10];

		ret.mt[5] = mt[0]  * mt[10] * mt[15] -
					mt[0]  * mt[11] * mt[14] -
					mt[8]  * mt[2]  * mt[15] +
					mt[8]  * mt[3]  * mt[14] +
					mt[12] * mt[2]  * mt[11] -
					mt[12] * mt[3]  * mt[10];

		ret.mt[9] = -mt[0]  * mt[9]  * mt[15] +
					 mt[0]  * mt[11] * mt[13] +
					 mt[8]  * mt[1]  * mt[15] -
					 mt[8]  * mt[3]  * mt[13] -
					 mt[12] * mt[1]  * mt[11] +
					 mt[12] * mt[3]  * mt[9];

		ret.mt[13] = mt[0]  * mt[9]  * mt[14] -
					 mt[0]  * mt[10] * mt[13] -
					 mt[8]  * mt[1]  * mt[14] +
					 mt[8]  * mt[2]  * mt[13] +
					 mt[12] * mt[1]  * mt[10] -
					 mt[12] * mt[2]  * mt[9];

		ret.mt[2] = mt[1]  * mt[6] * mt[15] -
					mt[1]  * mt[7] * mt[14] -
					mt[5]  * mt[2] * mt[15] +
					mt[5]  * mt[3] * mt[14] +
					mt[13] * mt[2] * mt[7] -
					mt[13] * mt[3] * mt[6];

		ret.mt[6] = -mt[0]  * mt[6] * mt[15] +
					 mt[0]  * mt[7] * mt[14] +
					 mt[4]  * mt[2] * mt[15] -
					 mt[4]  * mt[3] * mt[14] -
					 mt[12] * mt[2] * mt[7] +
					 mt[12] * mt[3] * mt[6];

		ret.mt[10] = mt[0]  * mt[5] * mt[15] -
					 mt[0]  * mt[7] * mt[13] -
					 mt[4]  * mt[1] * mt[15] +
					 mt[4]  * mt[3] * mt[13] +
					 mt[12] * mt[1] * mt[7] -
					 mt[12] * mt[3] * mt[5];

		ret.mt[14] = -mt[0]  * mt[5] * mt[14] +
					  mt[0]  * mt[6] * mt[13] +
					  mt[4]  * mt[1] * mt[14] -
					  mt[4]  * mt[2] * mt[13] -
					  mt[12] * mt[1] * mt[6] +
					  mt[12] * mt[2] * mt[5];

		ret.mt[3] = -mt[1] * mt[6] * mt[11] +
					 mt[1] * mt[7] * mt[10] +
					 mt[5] * mt[2] * mt[11] -
					 mt[5] * mt[3] * mt[10] -
					 mt[9] * mt[2] * mt[7] +
					 mt[9] * mt[3] * mt[6];

		ret.mt[7] = mt[0] * mt[6] * mt[11] -
					mt[0] * mt[7] * mt[10] -
					mt[4] * mt[2] * mt[11] +
					mt[4] * mt[3] * mt[10] +
					mt[8] * mt[2] * mt[7] -
					mt[8] * mt[3] * mt[6];

		ret.mt[11] = -mt[0] * mt[5] * mt[11] +
					  mt[0] * mt[7] * mt[9] +
					  mt[4] * mt[1] * mt[11] -
					  mt[4] * mt[3] * mt[9] -
					  mt[8] * mt[1] * mt[7] +
					  mt[8] * mt[3] * mt[5];

		ret.mt[15] = mt[0] * mt[5] * mt[10] -
					 mt[0] * mt[6] * mt[9] -
					 mt[4] * mt[1] * mt[10] +
					 mt[4] * mt[2] * mt[9] +
					 mt[8] * mt[1] * mt[6] -
					 mt[8] * mt[2] * mt[5];

		const float det = mt[0] * ret.mt[0] + mt[1] * ret.mt[4] + mt[2] * ret.mt[8] + mt[3] * ret.mt[12];
		ret /= det;
		return ret;
	}

	// 如果要找一個變換 T 的三個軸，先取 T.inverse 再取軸 (T 是 world 打到 local)
	inline vec3 x_axis() const {
		return vec3(mt[0],mt[4],mt[8]);
	}

	inline vec3 y_axis() const {
		return vec3(mt[1], mt[5], mt[9]);
	}

	inline vec3 z_axis() const {
		return vec3(mt[2], mt[6], mt[10]);
	}

	vec3 operator * (vec3 v) {
		return x_axis() * v.x + y_axis() * v.y + z_axis() * v.z + vec3(mt[3],mt[7],mt[11]);
	}

};


template<class T>
struct vector4 {
	T x, y, z, w;
	vector4(T _x, T _y, T _z, T _w) :x(_x), y(_y), z(_z),w(_w) {};
	vector4(): x(0), y(0), z(0),w(1) {};
	vector4(T* arr) :x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3]) {};
	vector4(vector3<T> vec,T _w = 1) :x(vec.x), y(vec.y), z(vec.z), w(_w) {};

	vector4 operator + (vector4 a) { return vector4(x + a.x, y + a.y, z + a.z,w + a.w); }
	vector4 operator - (vector4 a) { return vector4(x - a.x, y - a.y, z - a.z,w - a.w); }
	vector4 operator / (T c) { return vector4(x/c, y/c, z/c, w/c); }
	T operator * (vector4 a) { return (x * a.x + y * a.y + z * a.z,w * a.w); }
	inline void operator *= (T c) {
		x *= c, y *= c, z *= c, w *= c;
	}
	inline void operator /= (T c) {
		x /= c, y /= c, z /= c, w /= c;
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


using vec4 = vector4<ld>;
using mat4 = matrix4;

using vec2 = _Point2d<ld>;
ld abs2(vec3 a);
ld abs(vec3 a);
ld angle(vec3 a, vec3 b);
vec3 Rx(vec3 v, ld t);
vec3 Ry(vec3 v, ld t);
vec3 Rz(vec3 v, ld t);
vec3 rotate(vec3 v, vec3 axis,ld ang);
vec3 uni(vec3 a);
ld radToDegree(ld rad);
using std::numbers::pi;

#define TP(p) (p).x,(p).y,(p).z
