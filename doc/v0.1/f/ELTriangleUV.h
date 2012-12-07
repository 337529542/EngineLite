#pragma once
#include "ELVector2.h"

class ELTriangleUV
{
public:
	ELTriangleUV(){};
	ELTriangleUV(ELVector2 vv0, ELVector2 vv1, ELVector2 vv2):v0(vv0), v1(vv1), v2(vv2){};
	ELVector2 v0;
	ELVector2 v1;
	ELVector2 v2;
};