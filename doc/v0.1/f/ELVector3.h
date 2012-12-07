#pragma once

class ELVector3
{
public:
	ELVector3():x(0), y(0), z(0){};
	ELVector3(float xx, float yy, float zz):x(xx), y(yy), z(zz){};

	float x;
	float y;
	float z;
};