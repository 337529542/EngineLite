#include "ELCube.h"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

ELCube::ELCube( ELRenderer *renderer )
:m_Renderer(renderer)
{

	loadMesh();

	//Create IBuffer
	unsigned short ibuf[] = {2,1,0,3,4,5,6,7};
	IBuffer = m_Renderer->CreateIndexBuffer(CubeIBuf, vCount);

	//Create VBuffer
	float vbuf[] = 
	{
		0.0, 0.5, 0,
		0.1, 0.2, 0.3,
		1, 1,
		-0.5, -0.5, -0,
		0.4, 0.5, 0.6,
		0.0, 1.0,
		0.5, -0.5, 0,
		0.7, 0.8, 0.9,
		0, 0,
	};

	VBuffer = m_Renderer->CreateVertexBuffer(CubVBuf, vCount * 11);

	//set Matrix
	m_WorldMat.resetMatrix();
}

ELCube::~ELCube()
{
	m_Renderer->DeleteVertexBuffer(VBuffer);
	m_Renderer->DeleteIndexBuffer(IBuffer);
}

ELMatrix4x4* ELCube::GetWorldMatrix()
{
	return &m_WorldMat;
}

void ELCube::DrawMesh()
{
	m_Renderer->DrawMesh(IBuffer, VBuffer, vCount/3);
}

void ELCube::loadMesh()
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile( "Media\\AmmoBox.DAE", 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_MakeLeftHanded         |
		aiProcess_SortByPType);

	// If the import failed, report it
	if( !scene)
	{
		return;
	}
	// Now we can access the file's contents. 
	if(!scene->mNumMeshes)
	{
		return;
	}

	//always load the first mesh in dae scene
	aiMesh *pmesh = *(scene->mMeshes);

	vCount = pmesh->mNumFaces * 3;
	for(int i=0; i<vCount; i++)
	{
		CubeIBuf[i] = i;
	}

	for(int i=0; i<pmesh->mNumFaces; i++)
	{
		aiFace *pface = pmesh->mFaces + i;
		if(pface->mNumIndices != 3)
			continue;

		unsigned int v0ind = *pface->mIndices;
		unsigned int v1ind = *pface->mIndices + 1;
		unsigned int v2ind = *pface->mIndices + 2;

		//position
		aiVector3D *v0 = pmesh->mVertices + v0ind;
		aiVector3D *v1 = pmesh->mVertices + v1ind;
		aiVector3D *v2 = pmesh->mVertices + v2ind;

		CubVBuf[i*33 + 0] = v0->x;
		CubVBuf[i*33 + 1] = v0->y;
		CubVBuf[i*33 + 2] = v0->z;

		CubVBuf[i*33 + 11] = v1->x;
		CubVBuf[i*33 + 12] = v1->y;
		CubVBuf[i*33 + 13] = v1->z;

		CubVBuf[i*33 + 22] = v2->x;
		CubVBuf[i*33 + 23] = v2->y;
		CubVBuf[i*33 + 24] = v2->z;

		//normal
		aiVector3D *n0 = pmesh->mNormals + v0ind;
		aiVector3D *n1 = pmesh->mNormals + v1ind;
		aiVector3D *n2 = pmesh->mNormals + v2ind;

		CubVBuf[i*33 + 3] = n0->x;
		CubVBuf[i*33 + 4] = n0->y;
		CubVBuf[i*33 + 5] = n0->z;

		CubVBuf[i*33 + 14] = n1->x;
		CubVBuf[i*33 + 15] = n1->y;
		CubVBuf[i*33 + 16] = n1->z;

		CubVBuf[i*33 + 25] = n2->x;
		CubVBuf[i*33 + 26] = n2->y;
		CubVBuf[i*33 + 27] = n2->z;

		//TexCoord0
		aiVector3D *t0 = pmesh->mTextureCoords[0] + v0ind;
		aiVector3D *t1 = pmesh->mTextureCoords[0] + v1ind;
		aiVector3D *t2 = pmesh->mTextureCoords[0] + v2ind;

		CubVBuf[i*33 + 6] = t0->x;
		CubVBuf[i*33 + 7] = 1-t0->y;

		CubVBuf[i*33 + 17] = t1->x;
		CubVBuf[i*33 + 18] = 1-t1->y;

		CubVBuf[i*33 + 28] = t2->x;
		CubVBuf[i*33 + 29] = 1-t2->y;

		//Tangent
		aiVector3D *ta0 = pmesh->mTangents + v0ind;
		aiVector3D *ta1 = pmesh->mTangents + v1ind;
		aiVector3D *ta2 = pmesh->mTangents + v2ind;

		CubVBuf[i*33 + 8] = ta0->x;
		CubVBuf[i*33 + 9] = ta0->y;
		CubVBuf[i*33 + 10] = ta0->z;

		CubVBuf[i*33 + 19] = ta1->x;
		CubVBuf[i*33 + 20] = ta1->y;
		CubVBuf[i*33 + 21] = ta1->z;

		CubVBuf[i*33 + 30] = ta2->x;
		CubVBuf[i*33 + 31] = ta2->y;
		CubVBuf[i*33 + 32] = ta2->z;

	}
}