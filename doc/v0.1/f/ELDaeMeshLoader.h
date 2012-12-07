#pragma once
#include "ELMesh.h"

class ELDaeMeshLoader
{
public:
	static int loadMeshFromFile(char *fileName, ELMesh& mesh);//0=failed 1=ok
};