#pragma once

#include <D3D11.h>
#include <D3DX10.h>
#include <windows.h>

#define ELRenderer_GeometryVShader_FilePath "Media\\GeometryVertexShader.hlsl"
#define ELRenderer_GeometryVShader_Func "GeometryVertexShader"
#define ELRenderer_GeometryPShader_FilePath "Media\\GeometryPixelShader.hlsl"
#define ELRenderer_GeometryPShader_Func "GeometryPixelShader"

#define ELRenderer_DirectionalLightVShader_FilePath "Media\\DirectionalLightVertexShader.hlsl"
#define ELRenderer_DirectionalLightVShader_Func "DirectionalLightVertexShader"
#define ELRenderer_DirectionalLightPShader_FilePath "Media\\DirectionalLightPixelShader.hlsl"
#define ELRenderer_DirectionalLightPShader_Func "DirectionalLightPixelShader"

#define ELRenderer_Max_Vertex_Buffers 1000
#define ELRenderer_Max_Index_Buffers 1000
#define ELRenderer_Max_Texture2D 1000

#define ELRenderer_Max_DirectionalLight 100

class ELRenderer_ShaderVars_Geometry
{
public:
	float viewMatrix[4][4];
	float worldMatrix[4][4];
	float perspectiveMatrix[4][4];
};

class ELRenderer_DirectionalLight
{
public:
	int inuse;

	float r;
	float g;
	float b;
	float mult;

	float dx;
	float dy;
	float dz;

	ELRenderer_DirectionalLight()
	{
		inuse = 0;
		r = 1;
		g = 1;
		b = 1;
		mult = 1;

		dx = 1;
		dy = 1;
		dz = 1;
	}
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

	int CreateDirectionalLight(const ELRenderer_DirectionalLight *L);
	void DeleteDirectionalLight(int handle);

	void BeginGeometry();

		int SetGeometryConstant(const ELRenderer_ShaderVars_Geometry *constant);//returns -1 if failed
		void SetGeometryDiffuseTexture2D(int handle);
		void DrawMesh(const int IBuffer, const int VBuffer, int NumTriangles);

	void EndGeometryAndBeginLighting();

	void EndLightingAndBeginComposition();

	void EndConposition();

	//only for debug
	void BeginGeometryDebug();
	void EndGeometryDebug();
	void BeginLightingDebug();
	void EndLightingDebug();

private:
	void LoadGeometryVShader();
	void LoadGeometryPShader();
	void CreateMRT();
	void ReleaseMRT();

	void LoadLightingVShaders();
	void LoadLightingPShaders();

	void CreateDirectionalLightStuff();
	void ProcessDirectionalLights();

private:
	
	ID3D11Device*           m_pd3dDevice;
	ID3D11DeviceContext*	m_pd3dDeviceContext;
	ID3D11DepthStencilView* pDSV;

	int m_Width;
	int m_Height;

	//Screen
	IDXGISwapChain*         m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView_Screen;
	D3D11_VIEWPORT m_vp;

	//Geometry
	ID3D11Buffer *m_GeometryShaderVarsBuffer;
	ID3D11VertexShader *m_GeometryVShader;
	ID3D11PixelShader *m_GeometryPShader;
	ID3D11InputLayout *m_GeometryLayout;
	ID3D11SamplerState *m_GeometrySamplerState;
	ID3D11RasterizerState *m_GeometryRasterState;
	ID3D11DepthStencilState * m_GeometryDSState;

	//Vertex Buffers
	ID3D11Buffer *m_VertexBuffers[ELRenderer_Max_Vertex_Buffers];

	//Index Buffers
	ID3D11Buffer *m_IndexBuffers[ELRenderer_Max_Index_Buffers];

	//Texture2Ds
	ID3D11Resource *m_pTexture2Ds[ELRenderer_Max_Texture2D];
	ID3D11ShaderResourceView *m_pTex2DView[ELRenderer_Max_Texture2D];

	//MRT
	ID3D11Texture2D *mMRTTexture2D[4];
	ID3D11ShaderResourceView *MRTTexture2DSRV[4];
	ID3D11RenderTargetView *mMRTTexture2DRTV[4];

	//Lighting
	ID3D11SamplerState *m_LightingSamplerState;

		//Directional Light
	ELRenderer_DirectionalLight DirLight[ELRenderer_Max_DirectionalLight];
	ID3D11VertexShader *m_DirLightVShader;
	ID3D11PixelShader *m_DirLightPShader;
	ID3D11InputLayout *m_DirLightLayout;
	ID3D11RasterizerState *m_DirLightRState;
	ID3D11DepthStencilState * m_DirLightDSState;
	ID3D11Buffer *m_DirLightIndexBuffer;
	ID3D11Buffer *m_DirLightVertexBuffer;
};