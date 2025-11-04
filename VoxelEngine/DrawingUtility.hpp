#pragma once
#include <GL/freeglut.h>
#include "MathUtility.hpp"
struct Color {
	float r, g, b, a;
	Color(float _r, float _g, float _b, float _a = 1.0) :r(_r), g(_g), b(_b), a(_a) {}
	Color() :r(0.0), g(0.0), b(0.0), a(1.0) {}
	Color operator * (float  c) { return Color(r * c, g * c, b * c, a * c); }
	Color operator + (Color rst) { return Color(r + rst.r, g + rst.g, b + rst.b, a + rst.a); }
	friend Color blend(Color A, Color B, float p) {
		return A * p + B * (1.0 - p);
	}
	Color toGamma() const {
		return Color(sqrt(r), sqrt(g), sqrt(b), a);
	}
};
namespace PrimitiveShape {
	const float cube[8][3] = {
			{0, 0, 0},
			{1, 0, 0},
			{1, 1, 0},
			{0, 1, 0},
			{0, 0, 1},
			{1, 0, 1},
			{1, 1, 1},
			{0, 1, 1}
	};

	const int face[6][4] = { 
		{0, 3, 2, 1}, 
		{0, 1, 5, 4}, 
		{1, 2, 6, 5},
		{4, 5, 6, 7}, 
		{2, 3, 7, 6}, 
		{0, 4, 7, 3} 
	};

};

#define T4C(X) (X).r,(X).g,(X).b,(X).a
#define TC(X) (X).r,(X).g,(X).b

void draw_unicube();
void draw_unicylind();
void draw_unisphere();
void draw_unirectangle();
void drawAxis();
void alignedWithZ(vec3 axis);
void alignZTo(vec3 axis);