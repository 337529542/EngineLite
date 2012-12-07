#include "ELDaeMeshLoader.h"
#include "ELLog.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


int ELDaeMeshLoader::loadMeshFromFile( char *fileName, ELMesh& mesh )
{
	ELLog::getInstance()<<"ELDaeMeshLoader:Loading file"<<fileName<<endl;
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile( fileName, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_MakeLeftHanded         |
		aiProcess_SortByPType);

	// If the import failed, report it
	if( !scene)
	{
		//printf("error loading mesh %s: %s\n", fileName, importer.GetErrorString());
		ELLog::getInstance()<<"error loading mesh"<<fileName<<":"<<importer.GetErrorString()<<endl;
		return 0;
	}
	// Now we can access the file's contents. 
	if(!scene->mNumMeshes)
	{
		//printf("error there is no mesh.\n");
		ELLog::getInstance()<<"error there is no mesh.\n"<<endl;
	}

	//always load the first mesh in dae scene
	aiMesh *pmesh = *(scene->mMeshes);

	mesh.emptyMesh();

	for(int i=0; i<pmesh->mNumFaces; i++)
	{
		aiFace *pface = pmesh->mFaces + i;
		if(pface->mNumIndices != 3)
			continue;

		ELTriangle   triPoint;
		ELTriangle   triNormal;
		ELTriangleUV triUV;

		unsigned int v0ind = *pface->mIndices;
		unsigned int v1ind = *pface->mIndices + 1;
		unsigned int v2ind = *pface->mIndices + 2;

		//position
		aiVector3D *v0 = pmesh->mVertices + v0ind;
		aiVector3D *v1 = pmesh->mVertices + v1ind;
		aiVector3D *v2 = pmesh->mVertices + v2ind;

		triPoint.v0.x = v0->x;
		triPoint.v0.y = v0->y;
		triPoint.v0.z = v0->z;

		triPoint.v1.x = v2->x;
		triPoint.v1.y = v2->y;
		triPoint.v1.z = v2->z;

		triPoint.v2.x = v1->x;
		triPoint.v2.y = v1->y;
		triPoint.v2.z = v1->z;

		//normal
		aiVector3D *n0 = pmesh->mNormals + v0ind;
		aiVector3D *n1 = pmesh->mNormals + v1ind;
		aiVector3D *n2 = pmesh->mNormals + v2ind;

		triNormal.v0.x = n0->x;
		triNormal.v0.y = n0->y;
		triNormal.v0.z = n0->z;

		triNormal.v1.x = n2->x;
		triNormal.v1.y = n2->y;
		triNormal.v1.z = n2->z;

		triNormal.v2.x = n1->x;
		triNormal.v2.y = n1->y;
		triNormal.v2.z = n1->z;

		mesh.triPoint.push_back(triPoint);
		mesh.triNormal.push_back(triNormal);
		mesh.triUV.push_back(triUV);

	}

	return 1;
}