#pragma once
struct Spring {
	int a, b;
	float k = 10.0f;
	float length = -1.0f;
	// f = -kx 
};