#pragma once
#include "MathUtility.hpp"

class GameObject {
public:
	vec3 pos;
	vec3 rot;
	vec3 scale;

	GameObject():scale(1,1,1) {}
	virtual void draw() {}
	virtual void update() {}


	// local coordinate 's zaxis;
	vec3 forward() {
		return //���e��@��
			(mat4::Rz(rot.z) *
			 mat4::Ry(rot.y) *
			 mat4::Rx(rot.x) ).inverse().z_axis();
	}

	mat4 worldToLocal() {
		return 
			mat4::scale(1.0/scale) * 
			mat4::Rx(-rot.x) * // ��b�ন���
			mat4::Ry(-rot.y) *
			mat4::Rz(-rot.z) * 
			mat4::trans(-pos); // �첾
	}

	mat4 localToWorld() {
		return
			mat4::trans(pos) * // �첾
			mat4::Rz(rot.z) *
			mat4::Ry(rot.y) *
			mat4::Rx(rot.x) * // ��b�ন���
			mat4::scale(scale);
	}

	mat4 toWorldNoScale() { // �e����Ϊ�
		return
			mat4::trans(pos) * // �첾
			mat4::Rz(rot.z) *
			mat4::Ry(rot.y) *
			mat4::Rx(rot.x); // ��b�ন���
	}
};