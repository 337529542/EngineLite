#include "ELCube.h"

ELCube::ELCube( ELRenderer *renderer )
:m_Renderer(renderer)
{
	//Create IBuffer
	unsigned short ibuf[] = {2,1,0,3,4,5,6,7};
	IBuffer = m_Renderer->CreateIndexBuffer(ibuf, 8);

	//Create VBuffer
	float vbuf[] = 
	{
		0.0, 0.5, 0,
		0.0, 0.5, 0,
		-0.5, -0.5, -0,
		-0.5, -0.5, -0,
		0.5, -0.5, 0,
		0.5, -0.5, 0,
	};

	VBuffer = m_Renderer->CreateVertexBuffer(vbuf, 18);

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
	m_Renderer->DrawMesh(IBuffer, VBuffer, 1);
}