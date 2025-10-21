#pragma once
#include "MathUtility.hpp"
const ld eps = 1e-5;
int sgn(ld x) { return (x > -eps) - (x < eps); }
ld abs2(vec3 a) { return a.x * a.x + a.y * a.y + a.z * a.z; }
ld abs(vec3 a) { return sqrt(abs2(a)); }
vec3 uni(vec3 a) {
	ld l = abs(a);
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
vec3 rotate(vec3 v, vec3 axis, vec3 o, ld t) {
	vec3 zaxi(0, 0, 1);
	ld ofa = angle(axis, zaxi);
	v = v - o;
	v = Rx(v, -ofa);
	v = Ry(v, -ofa);
	v = Rz(v, t);
	v = Ry(v, ofa);
	v = Rx(v, ofa);
	v = v + o;
	return v;
}
ld radToDegree(ld rad) {
	return rad * 180 / pi;
}