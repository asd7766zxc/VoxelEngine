#pragma once
#include "MathUtility.hpp"
const ld eps = 1e-5;
int sgn(ld x) { return (x > -eps) - (x < eps); }
ld abs2(vec3 a) { return a.x * a.x + a.y * a.y + a.z * a.z; }
ld abs(vec3 a) { return sqrt(abs2(a)); }
vec3 uni(vec3 a) {
	ld l = abs(a);
	if (l < eps) return vec3();
	return vec3(a.x / l, a.y / l, a.z / l);
}
ld angle(vec3 a, vec3 b) {
	if (sgn(abs(a)) == 0 || sgn(abs(b)) == 0) return 0;
	return acos((a * b) / abs(a) / abs(b));
}
vec3 Rx(vec3 v, ld t) {
	ld c = cos(t);
	ld s = sin(t);
	auto [x, y, z] = v;
	ld ny = c * y - s * z;
	ld nz = s * y + c * z;
	return vec3(x, ny, nz);
}
vec3 Ry(vec3 v, ld t) {
	ld c = cos(t);
	ld s = sin(t);
	auto [x, y, z] = v;
	ld nz = c * z - s * x;
	ld nx = s * z + c * x;
	return vec3(nx, y, nz);
}
vec3 Rz(vec3 v, ld t) {
	ld c = cos(t);
	ld s = sin(t);
	auto [x, y, z] = v;
	ld nx = c * x - s * y;
	ld ny = s * x + c * y;
	return vec3(nx,ny,z);
}
vec3 rotate(vec3 v, vec3 axis,ld ang) {
	auto [x, y, z] = axis;
	ld _c = 1 - cos(ang);
	ld c = cos(ang);
	ld s = sin(ang);

	vec3 v0 = { x * x * _c + c    , x * y * _c + z * s ,x * z * _c - y * s };
	vec3 v1 = { x * y * _c - z * s, y * y * _c + c     ,y * z * _c + x * s };
	vec3 v2 = { x * z * _c + y * s, y * z * _c - x * s ,z * z * _c + c };
	vec3 ret = v0 * v.x + v1 * v.y + v2 * v.z;

	return ret;
}
ld radToDegree(ld rad) {
	return rad * 180 / pi;
}