#pragma once

#include "ELTriangle.h"
#include "ELTriangleUV.h"
#include <vector>

class ELMesh
{
public:
	std::vector<ELTriangle>   triPoint; 
	std::vector<ELTriangle>   triNormal;
	std::vector<ELTriangleUV> triUV;

	void emptyMesh()
	{
		triPoint.clear();
		triNormal.clear();
		triUV.clear();
	};

	ELMesh()
	{
		emptyMesh();
	};

	int getNumTriangles()
	{
		return triPoint.size();
	};
	
};