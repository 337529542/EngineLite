#include "ELMath.h"
#include "ELRenderer.h"

class ELCube
{
public:
	ELCube(ELRenderer *renderer);
	~ELCube();

	ELMatrix4x4* GetWorldMatrix();
	void DrawMesh();

private:
	void loadMesh();

private:
	ELRenderer *m_Renderer;
	ELMatrix4x4 m_WorldMat;

	int VBuffer;
	int IBuffer;

	float CubVBuf[20000];
	unsigned short CubeIBuf[20000];
	int vCount;
};