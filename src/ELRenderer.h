#pragma once

#include <D3D11.h>
#include <D3DX10.h>
#include <windows.h>

#define ELRenderer_GeometryVShader_FilePath "Media\\GeometryVertexShader.hlsl"
#define ELRenderer_GeometryVShader_Func "GeometryVertexShader"
#define ELRenderer_GeometryPShader_FilePath "Media\\GeometryPixelShader.hlsl"
#define ELRenderer_GeometryPShader_Func "GeometryPixelShader"

#define ELRenderer_Max_Vertex_Buffers 1000
#define ELRenderer_Max_Index_Buffers 1000
#define ELRenderer_Max_Texture2D 1000

class ELRenderer_ShaderVars_Geometry
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

	void Setup(HWND hWnd);
	void Shutdown();

	int CreateVertexBuffer(float *data, int numFloats);//returns -1 if failed
	void DeleteVertexBuffer(int handle);

	int CreateIndexBuffer(unsigned short *data, int numElements);//returns -1 if failed
	void DeleteIndexBuffer(int handle);

	int CreateTexture2D(char* filepath);//returns -1 if failed
	void DeleteTexture2D(int handle);
	

	void BeginGeometry();

		int SetGeometryConstant(const ELRenderer_ShaderVars_Geometry *constant);//returns -1 if failed
		void DrawMesh(const int IBuffer, const int VBuffer, int NumTriangles);

	void EndGeometryAndBeginLighting();

	void EndLightingAndBeginComposition();

	void EndConposition();

	//only for debug
	void BeginGeometryDebug();
	void EndGeometryDebug();

private:
	void LoadGeometryVShader();
	void LoadGeometryPShader();

private:
	
	ID3D11Device*           m_pd3dDevice;
	ID3D11DeviceContext*	m_pd3dDeviceContext;
	ID3D11DepthStencilView* pDSV;

	//Screen
	IDXGISwapChain*         m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView_Screen;
	D3D11_VIEWPORT m_vp;

	//Geometry
	ID3D11Buffer *m_GeometryShaderVarsBuffer;
	ID3D11VertexShader *m_GeometryVShader;
	ID3D11PixelShader *m_GeometryPShader;
	ID3D11InputLayout *m_GeometryLayout;

	//Vertex Buffers
	ID3D11Buffer *m_VertexBuffers[ELRenderer_Max_Vertex_Buffers];

	//Index Buffers
	ID3D11Buffer *m_IndexBuffers[ELRenderer_Max_Index_Buffers];

	//Texture2Ds
	ID3D11Resource *m_pTexture2Ds[ELRenderer_Max_Texture2D];
	ID3D11ShaderResourceView *m_pTex2DView[ELRenderer_Max_Texture2D];
};