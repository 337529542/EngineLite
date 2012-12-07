#include "ELRenderer.h"

#include <D3DX11async.h>
#include <d3d10shader.h>


ELRenderer::ELRenderer()
{

}

ELRenderer::~ELRenderer()
{

}

void ELRenderer::Setup( HWND hWnd )
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect( hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	UINT createDeviceFlags = 0;

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

	D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;

	D3D_FEATURE_LEVEL FeatureLevel;

	if( FAILED (hr = D3D11CreateDeviceAndSwapChain( NULL, 
		D3D_DRIVER_TYPE_REFERENCE,
		NULL, 
		D3D11_CREATE_DEVICE_DEBUG,
		&FeatureLevels, 
		1, 
		D3D11_SDK_VERSION, 
		&sd, 
		&m_pSwapChain, 
		&m_pd3dDevice, 
		&FeatureLevel,
		&m_pd3dDeviceContext )))
	{
		throw "D3D11CreateDeviceAndSwapChain";
	}

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
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

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;
	hr = m_pd3dDevice->CreateDepthStencilState(&dsDesc, &pDSState);
	if(FAILED(hr)) 
	{ 
		throw "CreateDepthStencilState"; 
	}

	// Bind depth stencil state
	m_pd3dDeviceContext->OMSetDepthStencilState(pDSState, 1);

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

	//Load Geometry VShader
	LoadGeometryVShader();

	//Load Geometry PShader
	LoadGeometryPShader();

}

void ELRenderer::Shutdown()
{

}

void ELRenderer::LoadGeometryVShader()
{
	HRESULT hr = S_OK;;

	//compile shader
	ID3D10Blob *pshader;
	ID3D10Blob *pErrmsg;
	hr = D3DX11CompileFromFile(ELRenderer_GeometryVShader_FilePath, NULL, NULL, ELRenderer_GeometryVShader_Func, "vs_4_0", 0, 0, NULL, &pshader, &pErrmsg, NULL);
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
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	hr = D3DX11CompileFromFile(ELRenderer_GeometryPShader_FilePath, NULL, NULL, ELRenderer_GeometryPShader_Func, "ps_4_0", 0, 0, NULL, &pshader, &pErrmsg, NULL);
	if( FAILED(hr) )
	{
		throw "D3DX11CompileFromFile";
	}
	else
	{
		//create vshader
		hr = m_pd3dDevice->CreatePixelShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), NULL, &m_GeometryPShader);
		if( FAILED(hr) )
		{
			throw "CreatePixelShader";
		}
	}
}