#pragma once
#include "ELVector3.h"

class ELTriangle
{
public:
	ELTriangle(){};
	ELTriangle(ELVector3 vv0, ELVector3 vv1, ELVector3 vv2):v0(vv0), v1(vv1), v2(vv2){};
	ELVector3 v0;
	ELVector3 v1;
	ELVector3 v2;
};