#pragma once
#include "ELEntity.h"
#include "ELMath.h"

#include <D3D11.h>
#include <D3DX10.h>
#include <windows.h>

#define MAX_VSHADERS 1000
#define MAX_PSHADERS 1000

class ELRenderer_ShaderVars
{
public:
	float viewMatrix[4][4];
	float worldMatrix[4][4];
	float perspectiveMatrix[4][4];
};

class ELRenderer
{
public:
	ELRenderer();
	~ELRenderer();

	void setup(HWND hWnd);

	int addVShader(char* shader, char* funcName);//return the index of the added shader, -1 if failed
	int addPShader(char* shader, char* funcName);

	void setRenderStage(int s);//0 and 1
	void setVarViewMatrix(ELMatrix4x4 *viewMatrix);
	void setPerspectiveMatrix(ELMatrix4x4 *perspectiveMatrix);

	void begin();
	void setVShader(int index);
	void setPShader(int index);
	void drawEntity(ELEntity *ent);
	void end();


private:
	ID3D11Device*           m_pd3dDevice;     //
	ID3D11DeviceContext*	m_pd3dDeviceContext;
	IDXGISwapChain*         m_pSwapChain;     //
	ID3D11RenderTargetView* m_pRenderTargetView;    //
	ID3D11DepthStencilView* pDSV; //depth

	int m_renderStage;

	ID3D11InputLayout*  m_InputLayout[MAX_VSHADERS];
	ID3D11VertexShader* m_VShader[MAX_VSHADERS];
	ID3D11PixelShader*  m_PShader[MAX_PSHADERS];

	//vars used for shaders
	ELMatrix4x4 m_viewMatrix;
	ELMatrix4x4 m_perspectiveMatrix;
	ID3D11Buffer *m_D3DViewMatrixBuffer;


};