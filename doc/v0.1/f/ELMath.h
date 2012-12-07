#pragma once
#include "ELVector3.h"


class ELMatrix4x4
{
public:
	//The matrix entries, indexed by [row][col].
	float m[4][4];

	void resetMatrix();

	void setTrans(const ELVector3& v);
	void setScale(const ELVector3& v);
	void setPitch(const float Radian);//x
	void setRoll(const float Radian);//z
	void setYaw(const float Radian);//y

	void mul(const ELMatrix4x4& m2);//src = src . m2


	void makePerspectiveMatrix(float FovY, float Aspect, float Zn, float Zf);

	ELMatrix4x4();
};