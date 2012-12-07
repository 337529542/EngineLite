#pragma once
#include "ELMath.h"
#include "ELMesh.h"

class ELEntity
{
public:
	ELMesh mesh;
	ELMatrix4x4 worldMatrix;

	ELEntity();
	void scale(const ELVector3& v);

};