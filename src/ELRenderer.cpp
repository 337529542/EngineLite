#include "ELRenderer.h"

#include <D3DX11async.h>
#include <d3d10shader.h>
#include <Dxerr.h>

#ifdef _DEBUG
void TraceC(LPCTSTR lpszFmt, ...)
{
	TCHAR szText[2000];
	va_list marker;
	va_start(marker, lpszFmt);
	wvsprintf(szText, lpszFmt, marker);
	va_end(marker);
	// 根据情况可采用下面任一种(或多种)输出形式
	OutputDebugString(szText);
	// cout<<szText<<endl;
	// printf(TEXT("%s\r\n"), szText);
	// WroteFile(hLogFile, ...
	// 其他


}
#define TRACE(fmt, sz) TraceC(fmt, sz)
#else
#define TRACE(fmt, sz)
#endif



ELRenderer::ELRenderer()
{
	
}

ELRenderer::~ELRenderer()
{
	
}

void ELRenderer::Setup( HWND hWnd )
{

//setup variables
	//clear m_VertexBuffers
	for(int i=0; i<ELRenderer_Max_Vertex_Buffers; i++)
		m_VertexBuffers[i] = 0;

	//clear m_IndexBuffers
	for(int i=0; i<ELRenderer_Max_Index_Buffers; i++)
		m_IndexBuffers[i] = 0;

	//clear texture 2d 
	for(int i=0; i<ELRenderer_Max_Texture2D; i++)
	{
		m_pTexture2Ds[i] = 0;
		m_pTex2DView[i] = 0;
	}

//setup system
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect( hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	UINT createDeviceFlags = 0;

	m_Width = width;
	m_Height = height;

	//setup Viewport
	m_vp.Width = width;
	m_vp.Height = height;
	m_vp.MinDepth = 0.0f;
	m_vp.MaxDepth = 1.0f;
	m_vp.TopLeftX = 0;
	m_vp.TopLeftY = 0;

	//Create device and swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;


	//D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_10_0;
	//D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_10_1;

	D3D_FEATURE_LEVEL FeatureLevel;

	if( FAILED (hr = D3D11CreateDeviceAndSwapChain( NULL, 
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, 
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		&FeatureLevels, 
		1, 
		D3D11_SDK_VERSION, 
		&sd, 
		&m_pSwapChain, 
		&m_pd3dDevice, 
		&FeatureLevel,
		&m_pd3dDeviceContext )))
	{
		DXTrace( __FILE__, __LINE__, hr, DXGetErrorDescription(hr), FALSE );
		throw "D3D11CreateDeviceAndSwapChain";
	}

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( pBackBuffer ), reinterpret_cast<void**>(&pBackBuffer) );
	if( FAILED( hr ) )
	{
		throw "GetBuffer";
		return;
	}
	hr = m_pd3dDevice->CreateRenderTargetView( pBackBuffer,NULL, &m_pRenderTargetView_Screen );
	pBackBuffer->Release();
	if( FAILED( hr ) )
	{
		throw "CreateRenderTargetView";
	}

	//Depth-Stencil
	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT ;//GetDXUTState().GetCurrentDeviceSettings()->AutoDepthStencilFormat;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D( &descDepth, NULL, &pDepthStencil );
	if(FAILED(hr)) 
	{
		throw "CreateTexture2D"; 
	}


	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state for Geometry stage
	hr = m_pd3dDevice->CreateDepthStencilState(&dsDesc, &m_GeometryDSState);
	if(FAILED(hr)) 
	{ 
		throw "CreateDepthStencilState"; 
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.Flags = 0;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	hr = m_pd3dDevice->CreateDepthStencilView( pDepthStencil, // Depth stencil texture
		&descDSV, // Depth stencil desc
		&pDSV );  // [out] Depth stencil view
	if(FAILED(hr)) 
	{ 
		throw "CreateDepthStencilView"; 
	}

	//Create Geometry Shader Vars Buffer
	D3D11_BUFFER_DESC GeometryShaderVarsBD;
	GeometryShaderVarsBD.Usage = D3D11_USAGE_DYNAMIC; 
	GeometryShaderVarsBD.ByteWidth = sizeof(ELRenderer_ShaderVars_Geometry); 
	GeometryShaderVarsBD.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	GeometryShaderVarsBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	GeometryShaderVarsBD.MiscFlags = 0; 
	GeometryShaderVarsBD.StructureByteStride = 0;

	hr = m_pd3dDevice->CreateBuffer(&GeometryShaderVarsBD, NULL, &m_GeometryShaderVarsBuffer); 
	if(FAILED(hr)) 
	{ 
		throw "CreateBuffer m_GeometryShaderVarsBuffer"; 
	}

	//Create Geometry Shader Vars Buffer Pixel
	D3D11_BUFFER_DESC GeometryShaderVarsBDPixel;
	GeometryShaderVarsBDPixel.Usage = D3D11_USAGE_DYNAMIC; 
	GeometryShaderVarsBDPixel.ByteWidth = sizeof(ELRenderer_ShaderVars_Geometry_Pixel); 
	GeometryShaderVarsBDPixel.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	GeometryShaderVarsBDPixel.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	GeometryShaderVarsBDPixel.MiscFlags = 0; 
	GeometryShaderVarsBDPixel.StructureByteStride = 0;

	hr = m_pd3dDevice->CreateBuffer(&GeometryShaderVarsBDPixel, NULL, &m_GeometryShaderVarsBufferPixel); 
	if(FAILED(hr)) 
	{ 
		throw "CreateBuffer m_GeometryShaderVarsBufferPixel"; 
	}

	
	//Load Geometry VShader
	LoadGeometryVShader();

	//Load Geometry PShader
	LoadGeometryPShader();

	//Create Sampler
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; 
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.MipLODBias = 0.0f; 
	samplerDesc.MaxAnisotropy = 1; 
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; 
	samplerDesc.BorderColor[0] = 0; 
	samplerDesc.BorderColor[1] = 0; 
	samplerDesc.BorderColor[2] = 0; 
	samplerDesc.BorderColor[3] = 0; 
	samplerDesc.MinLOD = 0; 
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_GeometrySamplerState);

	//Create Rasterizer State for Geometry stage
	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	m_pd3dDevice->CreateRasterizerState( &rasterizerState, &m_GeometryRasterState );
	if(FAILED(hr)) 
	{ 
		throw "CreateRasterizerState"; 
	}

	//Create MRT
	CreateMRT();

	//Init for Lighting...
	LoadLightingVShaders();
	LoadLightingPShaders();

	//Create Directional Light Shader Vars Buffer Pixel
	D3D11_BUFFER_DESC DirLightShaderVarsBDPixel;
	DirLightShaderVarsBDPixel.Usage = D3D11_USAGE_DYNAMIC; 
	DirLightShaderVarsBDPixel.ByteWidth = sizeof(ELRenderer_DirectionalLight_PixelShaderVars); 
	DirLightShaderVarsBDPixel.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	DirLightShaderVarsBDPixel.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	DirLightShaderVarsBDPixel.MiscFlags = 0; 
	DirLightShaderVarsBDPixel.StructureByteStride = 0;

	hr = m_pd3dDevice->CreateBuffer(&DirLightShaderVarsBDPixel, NULL, &m_DirLightShaderVarsBufferPixel); 
	if(FAILED(hr)) 
	{ 
		throw "CreateBuffer m_DirLightShaderVarsBufferPixel"; 
	}

		//Create Rasterizer State for Directional Light
	//D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = false;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	m_pd3dDevice->CreateRasterizerState( &rasterizerState, &m_DirLightRState );
	if(FAILED(hr)) 
	{ 
		throw "CreateRasterizerState"; 
	}

		//Create DepthStencilState for Directional lighting
	// Depth test parameters
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr = m_pd3dDevice->CreateDepthStencilState(&dsDesc, &m_DirLightDSState);
	if(FAILED(hr)) 
	{ 
		throw "CreateDepthStencilState"; 
	}

	//Create Sampler for Lighting
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; 
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.MipLODBias = 0.0f; 
	samplerDesc.MaxAnisotropy = 1; 
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; 
	samplerDesc.BorderColor[0] = 0; 
	samplerDesc.BorderColor[1] = 0; 
	samplerDesc.BorderColor[2] = 0; 
	samplerDesc.BorderColor[3] = 0; 
	samplerDesc.MinLOD = 0; 
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_LightingSamplerState);

		//Create Index and Vertex Buffers for Directional Lighting
	CreateDirectionalLightStuff();
}

void ELRenderer::Shutdown()
{
	//Release Lighting
	m_DirLightVShader->Release();
	m_DirLightPShader->Release();
	m_DirLightLayout->Release();
	m_DirLightRState->Release();

	//Release MRT
	ReleaseMRT();

	//Delete m_VertexBuffers
	for(int i=0; i<ELRenderer_Max_Vertex_Buffers; i++)
		if(m_VertexBuffers[i] != 0)
			DeleteVertexBuffer(i);

	//Delete m_IndexBuffers
	for(int i=0; i<ELRenderer_Max_Index_Buffers; i++)
		if(m_IndexBuffers[i] != 0)
			DeleteIndexBuffer(i);

	//Delete Texture2D
	for(int i=0; i<ELRenderer_Max_Texture2D; i++)
		if(m_pTexture2Ds[i] != 0)
			DeleteTexture2D(i);

	//Delete m_GeometryShaderVarsBuffer
	m_GeometryShaderVarsBuffer->Release();
	m_GeometryShaderVarsBufferPixel->Release();

	//Delete Geometry VShader
	m_GeometryVShader->Release();

	//Delete Geometry PShader
	m_GeometryPShader->Release();

	//Delete m_GeometryLayout
	m_GeometryLayout->Release();

	//Delete SamplerState
	m_GeometrySamplerState->Release();

	m_GeometryDSState->Release();

	m_GeometryRasterState->Release();


	//DirLight
	m_DirLightShaderVarsBufferPixel->Release();
}

void ELRenderer::LoadGeometryVShader()
{
	HRESULT hr = S_OK;;

	//compile shader
	ID3D10Blob *pshader;
	ID3D10Blob *pErrmsg;


	//for Pix debug
	UINT shaderFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	hr = D3DX11CompileFromFile(ELRenderer_GeometryVShader_FilePath, NULL, NULL, ELRenderer_GeometryVShader_Func, "vs_4_0", shaderFlags, 0, NULL, &pshader, &pErrmsg, NULL);
	if( FAILED(hr) )
	{
		throw "D3DX11CompileFromFile";
	}
	else
	{
		//create vshader
		hr = m_pd3dDevice->CreateVertexShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), NULL, &m_GeometryVShader);
		if( FAILED(hr) )
		{
			throw "CreateVertexShader";
		}
		else
		{
			//create layout
			const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			hr = m_pd3dDevice->CreateInputLayout(
				basicVertexLayoutDesc,
				ARRAYSIZE(basicVertexLayoutDesc),
				pshader->GetBufferPointer(),
				pshader->GetBufferSize(),
				&m_GeometryLayout
				);

			if( FAILED(hr) )
			{
				throw "CreateInputLayout";
			}
		}
	}
}

void ELRenderer::LoadGeometryPShader()
{
	HRESULT hr = S_OK;;

	//compile shader
	ID3D10Blob *pshader;
	ID3D10Blob *pErrmsg;

	//for Pix debug
	UINT shaderFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	hr = D3DX11CompileFromFile(ELRenderer_GeometryPShader_FilePath, NULL, NULL, ELRenderer_GeometryPShader_Func, "ps_4_0", shaderFlags, 0, NULL, &pshader, &pErrmsg, NULL);
	if( FAILED(hr) )
	{
		TRACE("%s", pErrmsg->GetBufferPointer());
		throw "D3DX11CompileFromFile";
	}
	else
	{
		//create pshader
		hr = m_pd3dDevice->CreatePixelShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), NULL, &m_GeometryPShader);
		if( FAILED(hr) )
		{
			throw "CreatePixelShader";
		}
	}
}

int ELRenderer::CreateVertexBuffer( float *data, int numFloats )
{
	int EmptySlot = -1;

	//find a empty slot
	for(int i=0; i<ELRenderer_Max_Vertex_Buffers; i++)
	{
		if(m_VertexBuffers[i] == 0)
		{
			EmptySlot = i;
			break;
		}
	}
	
	if(EmptySlot == -1)
		return -1;


	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( float ) * numFloats;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;



	HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_VertexBuffers[EmptySlot]);
	if( FAILED(hr) )
	{
		m_VertexBuffers[EmptySlot] = 0;
		return -1;
	}

	return EmptySlot;
}

void ELRenderer::DeleteVertexBuffer( int handle )
{
	if(m_VertexBuffers[handle] != 0)
	{
		m_VertexBuffers[handle]->Release();
		m_VertexBuffers[handle] = 0;
	}
}

int ELRenderer::CreateIndexBuffer( unsigned short *data, int numElements )
{
	int EmptySlot = -1;

	//find a empty slot
	for(int i=0; i<ELRenderer_Max_Index_Buffers; i++)
	{
		if(m_IndexBuffers[i] == 0)
		{
			EmptySlot = i;
			break;
		}
	}

	if(EmptySlot == -1)
		return -1;


	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * numElements;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = data;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	HRESULT hr = m_pd3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_IndexBuffers[EmptySlot]);
	if( FAILED(hr) )
	{
		m_IndexBuffers[EmptySlot] = 0;
		return -1;
	}

	return EmptySlot;
}

void ELRenderer::DeleteIndexBuffer( int handle )
{
	if(m_IndexBuffers[handle] != 0)
	{
		m_IndexBuffers[handle]->Release();
		m_IndexBuffers[handle] = 0;
	}
}

int ELRenderer::SetGeometryConstant( const ELRenderer_ShaderVars_Geometry *constant )
{
	ELRenderer_ShaderVars_Geometry *shaderVarsMapPtr;

	D3D11_MAPPED_SUBRESOURCE GeometryShaderMappedResource;
	HRESULT hr = m_pd3dDeviceContext->Map(m_GeometryShaderVarsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &GeometryShaderMappedResource); 
	if(FAILED(hr)) 
	{ 
		return -1;
	}

	shaderVarsMapPtr = (ELRenderer_ShaderVars_Geometry *)GeometryShaderMappedResource.pData;

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			shaderVarsMapPtr->viewMatrix[i][j] = constant->viewMatrix[j][i];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			shaderVarsMapPtr->worldMatrix[i][j] =constant->worldMatrix[j][i];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			shaderVarsMapPtr->perspectiveMatrix[i][j] = constant->perspectiveMatrix[j][i];

	m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_GeometryShaderVarsBuffer);
	m_pd3dDeviceContext->Unmap(m_GeometryShaderVarsBuffer, 0);

	return 0;
}

int ELRenderer::SetGeometryPixelConstant(const ELRenderer_ShaderVars_Geometry_Pixel *constant)
{
	ELRenderer_ShaderVars_Geometry_Pixel *shaderVarsMapPtr;

	D3D11_MAPPED_SUBRESOURCE GeometryShaderMappedResource;
	HRESULT hr = m_pd3dDeviceContext->Map(m_GeometryShaderVarsBufferPixel, 0, D3D11_MAP_WRITE_DISCARD, 0, &GeometryShaderMappedResource); 
	if(FAILED(hr)) 
	{ 
		return -1;
	}

	shaderVarsMapPtr = (ELRenderer_ShaderVars_Geometry_Pixel *)GeometryShaderMappedResource.pData;

	shaderVarsMapPtr->K_a = constant->K_a;
	shaderVarsMapPtr->K_d = constant->K_d;
	shaderVarsMapPtr->K_s = constant->K_s;
	shaderVarsMapPtr->Ns = constant->Ns;

	m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_GeometryShaderVarsBufferPixel);
	m_pd3dDeviceContext->Unmap(m_GeometryShaderVarsBufferPixel, 0);

	return 0;
}

void ELRenderer::DrawMesh(const int IBuffer, const int VBuffer, int NumTriangles)
{
	UINT stride = sizeof(float) * 11;
	UINT offset = 0;

	m_pd3dDeviceContext->IASetVertexBuffers( 0, 1, &m_VertexBuffers[VBuffer], &stride, &offset );
	m_pd3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	m_pd3dDeviceContext->IASetIndexBuffer(m_IndexBuffers[IBuffer], DXGI_FORMAT_R16_UINT, 0);

	//m_pd3dDeviceContext->PSSetShaderResources(0, 1, )

	m_pd3dDeviceContext->DrawIndexed(NumTriangles*3, 0, 0);
}

void ELRenderer::BeginGeometryDebug()
{
	ID3D11RenderTargetView *mrt[4];
	//mrt[0] = m_pRenderTargetView_Screen;
	mrt[0] = mMRTTexture2DRTV[0];
	mrt[1] = mMRTTexture2DRTV[1];
	mrt[2] = mMRTTexture2DRTV[2];
	mrt[3] = mMRTTexture2DRTV[3];

	m_pd3dDeviceContext->OMSetRenderTargets( 4, mrt, pDSV );

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //red,green,blue,alpha
	float ClearColorPos[4] = { 0.0f, 0.0f, 0.0f, -1.0f }; //red,green,blue,alpha
	m_pd3dDeviceContext->ClearRenderTargetView( mrt[0], ClearColor );
	m_pd3dDeviceContext->ClearRenderTargetView( mrt[1], ClearColor );
	m_pd3dDeviceContext->ClearRenderTargetView( mrt[2], ClearColor );
	m_pd3dDeviceContext->ClearRenderTargetView( mrt[3], ClearColorPos );

	m_pd3dDeviceContext->OMSetDepthStencilState(m_GeometryDSState, 1);

	m_pd3dDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pd3dDeviceContext->VSSetShader(m_GeometryVShader, NULL, 0);
	m_pd3dDeviceContext->IASetInputLayout(m_GeometryLayout);

	m_pd3dDeviceContext->PSSetShader(m_GeometryPShader, NULL, 0);

	m_pd3dDeviceContext->RSSetViewports( 1, &m_vp );

	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_GeometrySamplerState);

	m_pd3dDeviceContext->RSSetState(m_GeometryRasterState);

}

void ELRenderer::EndGeometryDebug()
{
	//clear the texture resources.
	ID3D11ShaderResourceView *RSNULL = NULL;
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &RSNULL);
	m_pd3dDeviceContext->PSSetShaderResources(1, 1, &RSNULL);
	m_pd3dDeviceContext->PSSetShaderResources(2, 1, &RSNULL);
	m_pd3dDeviceContext->PSSetShaderResources(3, 1, &RSNULL);
}

int ELRenderer::CreateTexture2D( char* filepath )
{
	int resindex = -1;

	for(int i=0; i< ELRenderer_Max_Texture2D; i++)
	{
		if(m_pTexture2Ds[i] == 0)
		{
			resindex = i;
			break;
		}
	}

	if(resindex == -1)
		return -1;

	D3DX11_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory( &loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO) );
	loadInfo.Width = D3DX11_DEFAULT;
	loadInfo.Height = D3DX11_DEFAULT;
	loadInfo.Depth = D3DX11_DEFAULT;
	loadInfo.FirstMipLevel = D3DX11_DEFAULT;
	loadInfo.MipLevels = D3DX11_DEFAULT;
	loadInfo.Usage = (D3D11_USAGE) D3DX11_DEFAULT;
	loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags = D3DX11_DEFAULT;
	loadInfo.MiscFlags = D3DX11_DEFAULT;
	loadInfo.Format = DXGI_FORMAT_FROM_FILE;
	loadInfo.Filter = D3DX11_DEFAULT;
	loadInfo.MipFilter = D3DX11_DEFAULT;
	loadInfo.pSrcInfo = NULL;

	HRESULT hr = D3DX11CreateTextureFromFile(m_pd3dDevice, filepath, &loadInfo, NULL, &m_pTexture2Ds[resindex], NULL);
	if(FAILED(hr)) 
	{
		if(hr == D3D11_ERROR_FILE_NOT_FOUND)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == D3DERR_INVALIDCALL)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == D3DERR_WASSTILLDRAWING)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == E_FAIL)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == E_INVALIDARG)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == E_OUTOFMEMORY)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == S_FALSE)
			throw "D3DX11CreateTextureFromFile";
		else if(hr == S_OK)
			throw "D3DX11CreateTextureFromFile";

		m_pTexture2Ds[resindex] = 0;
		return -1;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_TEXTURE2D_DESC desc;
	ID3D11Texture2D *pTexture2D = (ID3D11Texture2D*)m_pTexture2Ds[resindex];
	pTexture2D->GetDesc( &desc );

	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = m_pd3dDevice->CreateShaderResourceView( m_pTexture2Ds[resindex], &srvDesc, &m_pTex2DView[resindex] );
	if(FAILED(hr)) 
	{
		throw "CreateShaderResourceView";
		m_pTexture2Ds[resindex]->Release();
		m_pTexture2Ds[resindex] = 0;
		m_pTex2DView[resindex] = 0;
	}

	return resindex;
}

void ELRenderer::DeleteTexture2D(int handle)
{
	m_pTex2DView[handle]->Release();
	m_pTexture2Ds[handle]->Release();

	m_pTexture2Ds[handle] = 0;
	m_pTex2DView[handle] = 0;
}

void ELRenderer::SetGeometryDiffuseTexture2D( int handle )
{
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pTex2DView[handle]);
}

void ELRenderer::SetGeometryNormalTexture2D( int handle )
{
	m_pd3dDeviceContext->PSSetShaderResources(1, 1, &m_pTex2DView[handle]);
}

void ELRenderer::SetGeometrySpecularTexture2D( int handle )
{
	m_pd3dDeviceContext->PSSetShaderResources(2, 1, &m_pTex2DView[handle]);
}

void ELRenderer::CreateMRT()
{
	//Create Texture2D
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = NULL;
	desc.MiscFlags = 0;

	HRESULT hr = m_pd3dDevice->CreateTexture2D( &desc, NULL, &mMRTTexture2D[0] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateTexture2D 1";
	}

	hr = m_pd3dDevice->CreateTexture2D( &desc, NULL, &mMRTTexture2D[1] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateTexture2D 2";
	}

	hr = m_pd3dDevice->CreateTexture2D( &desc, NULL, &mMRTTexture2D[2] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateTexture2D 3";
	}

	hr = m_pd3dDevice->CreateTexture2D( &desc, NULL, &mMRTTexture2D[3] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateTexture2D 4";
	}

	//Create Shader resource viwe
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = m_pd3dDevice->CreateShaderResourceView( mMRTTexture2D[0], &srvDesc, &MRTTexture2DSRV[0] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateShaderResourceView 1";
	}

	hr = m_pd3dDevice->CreateShaderResourceView( mMRTTexture2D[1], &srvDesc, &MRTTexture2DSRV[1] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateShaderResourceView 2";
	}

	hr = m_pd3dDevice->CreateShaderResourceView( mMRTTexture2D[2], &srvDesc, &MRTTexture2DSRV[2] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateShaderResourceView 3";
	}

	hr = m_pd3dDevice->CreateShaderResourceView( mMRTTexture2D[3], &srvDesc, &MRTTexture2DSRV[3] );
	if(FAILED(hr)) 
	{
		throw "MRT CreateShaderResourceView 4";
	}

	//Create Render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtvdesc;
	rtvdesc.Format=desc.Format;
	rtvdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvdesc.Texture2D.MipSlice = 0;

	hr = m_pd3dDevice->CreateRenderTargetView(mMRTTexture2D[0], &rtvdesc, &mMRTTexture2DRTV[0]);
	if(FAILED(hr)) 
	{
		throw "MRT CreateRenderTargetView 1";
	}

	hr = m_pd3dDevice->CreateRenderTargetView(mMRTTexture2D[1], &rtvdesc, &mMRTTexture2DRTV[1]);
	if(FAILED(hr)) 
	{
		throw "MRT CreateRenderTargetView 2";
	}

	hr = m_pd3dDevice->CreateRenderTargetView(mMRTTexture2D[2], &rtvdesc, &mMRTTexture2DRTV[2]);
	if(FAILED(hr)) 
	{
		throw "MRT CreateRenderTargetView 3";
	}

	hr = m_pd3dDevice->CreateRenderTargetView(mMRTTexture2D[3], &rtvdesc, &mMRTTexture2DRTV[3]);
	if(FAILED(hr)) 
	{
		throw "MRT CreateRenderTargetView 4";
	}
}

void ELRenderer::ReleaseMRT()
{
	for(int i=0; i<4; i++)
	{
		mMRTTexture2DRTV[i]->Release();
		MRTTexture2DSRV[i]->Release();
		mMRTTexture2D[i]->Release();
	}
}

void ELRenderer::BeginLightingDebug()
{
	ID3D11RenderTargetView *mrt[1];
	mrt[0] = m_pRenderTargetView_Screen;

	m_pd3dDeviceContext->OMSetRenderTargets( 1, mrt, pDSV );

	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //red,green,blue,alpha
	m_pd3dDeviceContext->ClearRenderTargetView( mrt[0], ClearColor );

	ProcessDirectionalLights();
}

void ELRenderer::EndLightingDebug()
{
	m_pSwapChain->Present( 0, 0 );
}

void ELRenderer::LoadLightingVShaders()
{
	HRESULT hr = S_OK;;


	//Load DirectionalLight VShader
	//compile shader
	ID3D10Blob *pshader;
	ID3D10Blob *pErrmsg;


	//for Pix debug
	UINT shaderFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	hr = D3DX11CompileFromFile(ELRenderer_DirectionalLightVShader_FilePath, NULL, NULL, ELRenderer_DirectionalLightVShader_Func, "vs_4_0", shaderFlags, 0, NULL, &pshader, &pErrmsg, NULL);
	if( FAILED(hr) )
	{
		throw "D3DX11CompileFromFile";
	}
	else
	{
		//create vshader
		hr = m_pd3dDevice->CreateVertexShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), NULL, &m_DirLightVShader);
		if( FAILED(hr) )
		{
			throw "CreateVertexShader";
		}
		else
		{
			//create layout
			const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			hr = m_pd3dDevice->CreateInputLayout(
				basicVertexLayoutDesc,
				ARRAYSIZE(basicVertexLayoutDesc),
				pshader->GetBufferPointer(),
				pshader->GetBufferSize(),
				&m_DirLightLayout
				);

			if( FAILED(hr) )
			{
				throw "CreateInputLayout";
			}
		}
	}
}

void ELRenderer::LoadLightingPShaders()
{
	HRESULT hr = S_OK;;

	//Load DirectionalLight PShader
	//compile shader
	ID3D10Blob *pshader;
	ID3D10Blob *pErrmsg;

	//for Pix debug
	UINT shaderFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	hr = D3DX11CompileFromFile(ELRenderer_DirectionalLightPShader_FilePath, NULL, NULL, ELRenderer_DirectionalLightPShader_Func, "ps_4_0", shaderFlags, 0, NULL, &pshader, &pErrmsg, NULL);
	if( FAILED(hr) )
	{
		TRACE("%s", pErrmsg->GetBufferPointer());
		throw "D3DX11CompileFromFile";
	}
	else
	{
		//create vshader
		hr = m_pd3dDevice->CreatePixelShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), NULL, &m_DirLightPShader);
		if( FAILED(hr) )
		{
			throw "CreatePixelShader";
		}
	}
}

void ELRenderer::ProcessDirectionalLights()
{
	//just for test, process one test light
	m_pd3dDeviceContext->OMSetDepthStencilState(m_DirLightDSState, 1);

	m_pd3dDeviceContext->VSSetShader(m_DirLightVShader, NULL, 0);
	m_pd3dDeviceContext->IASetInputLayout(m_DirLightLayout);
	m_pd3dDeviceContext->PSSetShader(m_DirLightPShader, NULL, 0);
	m_pd3dDeviceContext->RSSetViewports( 1, &m_vp );

	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_LightingSamplerState);
	m_pd3dDeviceContext->RSSetState(m_DirLightRState);

	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &MRTTexture2DSRV[0]);
	m_pd3dDeviceContext->PSSetShaderResources(1, 1, &MRTTexture2DSRV[1]);
	m_pd3dDeviceContext->PSSetShaderResources(2, 1, &MRTTexture2DSRV[2]);
	m_pd3dDeviceContext->PSSetShaderResources(3, 1, &MRTTexture2DSRV[3]);

	//set constant buffer////////////////////////////////////////////////////////////
	ELRenderer_DirectionalLight_PixelShaderVars *shaderVarsMapPtr;

	D3D11_MAPPED_SUBRESOURCE DirLightShaderMappedResource;
	HRESULT hr = m_pd3dDeviceContext->Map(m_DirLightShaderVarsBufferPixel, 0, D3D11_MAP_WRITE_DISCARD, 0, &DirLightShaderMappedResource); 
	if(FAILED(hr)) 
	{ 
		throw "ProcessDirectionalLights";
	}

	shaderVarsMapPtr = (ELRenderer_DirectionalLight_PixelShaderVars *)DirLightShaderMappedResource.pData;

	shaderVarsMapPtr->lightcolor[0] = 0.8;
	shaderVarsMapPtr->lightcolor[1] = 0.8;
	shaderVarsMapPtr->lightcolor[2] = 0.8;
	shaderVarsMapPtr->lightcolor[3] = 1;

	shaderVarsMapPtr->lightrdir[0] = -200;
	shaderVarsMapPtr->lightrdir[1] = 0;
	shaderVarsMapPtr->lightrdir[2] = -100;
	shaderVarsMapPtr->lightrdir[3] = 0;

	shaderVarsMapPtr->ViewPosInWorld[0] = 0;
	shaderVarsMapPtr->ViewPosInWorld[1] = 0;
	shaderVarsMapPtr->ViewPosInWorld[2] = -70;
	shaderVarsMapPtr->ViewPosInWorld[3] = 1;

	m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_DirLightShaderVarsBufferPixel);
	m_pd3dDeviceContext->Unmap(m_DirLightShaderVarsBufferPixel, 0);
	/////////////////////////////////////////////////////////////////////////////////

	UINT stride = sizeof(float) * 5;
	UINT offset = 0;

	m_pd3dDeviceContext->IASetVertexBuffers( 0, 1, &m_DirLightVertexBuffer, &stride, &offset );
	m_pd3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	m_pd3dDeviceContext->IASetIndexBuffer(m_DirLightIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	m_pd3dDeviceContext->DrawIndexed(6, 0, 0);

	ID3D11ShaderResourceView *RSNULL = NULL;
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &RSNULL);
	m_pd3dDeviceContext->PSSetShaderResources(1, 1, &RSNULL);
	m_pd3dDeviceContext->PSSetShaderResources(2, 1, &RSNULL);
	m_pd3dDeviceContext->PSSetShaderResources(3, 1, &RSNULL);


}

void ELRenderer::CreateDirectionalLightStuff()
{
	float vbuf[20] = {
		-1, 1, 0.5f,
		0, 1,
		1, 1, 0.5f,
		1, 1,
		-1, -1, 0.5f,
		0, 0,
		1, -1, 0.5f,
		1, 0,
	};

	unsigned short ibuf[6] = {
		0, 2, 1,
		1, 2, 3,
	};

	//Vertex Buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( float ) * 20;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vbuf;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	HRESULT hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_DirLightVertexBuffer);
	if( FAILED(hr) )
	{
		throw "CreateDirectionalLightBuffers";
	}

	//Index Buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * 6;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = ibuf;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	hr = m_pd3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_DirLightIndexBuffer);
	if( FAILED(hr) )
	{
		throw "CreateDirectionalLightBuffers";
	}
}
