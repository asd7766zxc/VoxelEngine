#pragma once
struct Spring {
	int a, b;
	float k = 10.0f;
	float length = -1.0f;
	float partial = 0.8f; // �̦h�ಣ�ͦh�֧���
	// f = -kx 
};