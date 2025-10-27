#pragma once
#include <cmath>
#include <array>
#include <vector>
#include <numbers>
#include <ostream>

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
	vector3 operator / (T a) { return vector3(x / a, y / a, z / a); }
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
	friend vector3 operator / (T a, vector3 v) { return vector3(a / v.x , a / v.y, a / v.z ); };

};

template<class T>
struct vector4 {
public:
	T x, y, z, w;
	vector4(T _x, T _y, T _z, T _w) :x(_x), y(_y), z(_z), w(_w) {};
	vector4() : x(0), y(0), z(0), w(1) {};
	vector4(T* arr) :x(arr[0]), y(arr[1]), z(arr[2]), w(arr[3]) {};
	vector4(vector3<T> vec, T _w = 1) :x(vec.x), y(vec.y), z(vec.z), w(_w) {};

	vector3<T> toVec3() { return vector3<T>(x, y, z); }
	vector4 operator + (vector4 a) { return vector4(x + a.x, y + a.y, z + a.z, w + a.w); }
	vector4 operator - (vector4 a) { return vector4(x - a.x, y - a.y, z - a.z, w - a.w); }
	vector4 operator / (T c) { return vector4(x / c, y / c, z / c, w / c); }
	T operator * (vector4 a) { return (x * a.x + y * a.y + z * a.z, w * a.w); }
	vector4 operator * (T a) { return vector4(x * a,y * a,z * a,w * a); }

	inline void operator *= (T c) {
		x *= c, y *= c, z *= c, w *= c;
	}
	inline void operator /= (T c) {
		x /= c, y /= c, z /= c, w /= c;
	}
};


using ld = float;
using vec3 = vector3<ld>;
using vec4 = vector4<ld>;

using std::numbers::pi;

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

	matrix4 operator * (matrix4 a) const {
		matrix4 ret;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				for (int k = 0; k < 4; ++k) {
					ret[i * 4 + j] += mt[i * 4 + k] * a[k * 4 + j];
				}
		return ret;
	}

	vec4 operator * (vec4 v) {
		vec4 ret;
		ret.x = mt[0]  * v.x + mt[1]  * v.y + mt[2]  * v.z + mt[3]  * v.w;
		ret.y = mt[4]  * v.x + mt[5]  * v.y + mt[6]  * v.z + mt[7]  * v.w;
		ret.z = mt[8]  * v.x + mt[9]  * v.y + mt[10] * v.z + mt[11] * v.w;
		ret.w = mt[12] * v.x + mt[13] * v.y + mt[14] * v.z + mt[15] * v.w;
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
		mt[0]  = s.x;  mt[1]  = 0.0f; mt[2]  = 0.0f; mt[3]  = 0.0f;
		mt[4]  = 0.0f; mt[5]  = s.y;  mt[6]  = 0.0f; mt[7]  = 0.0f;
		mt[8]  = 0.0f; mt[9]  = 0.0f; mt[10] = s.z;  mt[11] = 0.0f;
		mt[12] = 0.0f; mt[13] = 0.0f; mt[14] = 0.0f; mt[15] = 1.0f;
	}

	void makeIdentity() {
		makeZero();
		for (int i = 0; i < 4; ++i) mt[i * 4 + i] = 1;
	}
	void makeZero() {
		std::fill(mt, mt + 16, 0);
	}

	inline static matrix4 zero()		{ matrix4 m; m.makeZero();	     return m; }
	inline static matrix4 Rx(float a)	{ matrix4 m; m.makeRX(a);	     return m; }
	inline static matrix4 Ry(float a)	{ matrix4 m; m.makeRY(a);		 return m; }
	inline static matrix4 Rz(float a)	{ matrix4 m; m.makeRZ(a);		 return m; }
	inline static matrix4 trans(vec3 t) { matrix4 m; m.makeTrans(t);	 return m; }
	inline static matrix4 scale(vec3 s) { matrix4 m; m.makeScale(s);	 return m; }
	inline static matrix4 identity()	{ matrix4 m; m.makeIdentity();   return m; }

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
	// local 打到 world 就直接取軸
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

	matrix4 rotate(vec3 axis, float ang) const {
		matrix4 m;
		auto [x, y, z] = axis;
		ld _c = 1 - cos(ang);
		ld c = cos(ang);
		ld s = sin(ang);
		std::vector<vec3> v = {
			{ x * x * _c + c    , x * y * _c + z * s ,x * z * _c - y * s },
			{ x * y * _c - z * s, y * y * _c + c     ,y * z * _c + x * s },
			{ x * z * _c + y * s, y * z * _c - x * s ,z * z * _c + c }
		};
		for (int i = 0; i < 3; ++i) {
			m[0 + i] = v[i][0];
			m[4 + i] = v[i][1];
			m[8 + i] = v[i][3];
		}
		m[3 * 4 + 3] = 1;
		return m;
	}

	//https://eecs.qmul.ac.uk/~gslabaugh/publications/euler.pdf
	//旋轉分解
	vec3 toEulerAngles() const {
		vec3 ret;
		auto& [psi, theta, phi] = ret;
		if ((mt[3 * 4 + 1] != +1) && (mt[3 * 4 + 1] != -1)) {
			theta = -std::asin(mt[3 * 4 + 1]);
			auto c = std::cos(theta);
			psi = std::atan2(mt[3 * 4 + 2] / c, mt[3 * 4 + 3] / c);
			phi = std::atan2(mt[2 * 4 + 1] / c, mt[1 * 4 + 1] / c);
		}
		else {
			if (mt[3 * 4 + 1] == -1) {
				theta = pi / 2;
				psi = phi + std::atan2(mt[1 * 4 + 2], mt[1 * 4 + 3]);
			}
			else {
				theta = -pi / 2;
				psi = -phi + std::atan2(-mt[1 * 4 + 2], -mt[1 * 4 + 3]);
			}
		}
		return ret;
	}
};

using mat4 = matrix4;


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

// I/O
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	out << v.x << ", " << v.y << ", " << v.z;
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const vec4& v) {
	out << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const mat4& m) {

	out << m.mt[0] << ", " << m.mt[1] << ", " << m.mt[2] << ", " << m.mt[3] << "\n";
	out << m.mt[4] << ", " << m.mt[5] << ", " << m.mt[6] << ", " << m.mt[7] << "\n";
	out << m.mt[8] << ", " << m.mt[9] << ", " << m.mt[10] << ", " << m.mt[11] << "\n";
	out << m.mt[12] << ", " << m.mt[13] << ", " << m.mt[14] << ", " << m.mt[15] << "\n";
	return out;
}



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
int sgn(ld x);
const ld eps = 1e-5;
#define TP(p) (p).x,(p).y,(p).z
