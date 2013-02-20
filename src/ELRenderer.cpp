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

//setup variables
	//clear m_VertexBuffers
	for(int i=0; i<ELRenderer_Max_Vertex_Buffers; i++)
		m_VertexBuffers[i] = 0;

	//clear m_IndexBuffers
	for(int i=0; i<ELRenderer_Max_Index_Buffers; i++)
		m_IndexBuffers[i] = 0;

//setup system
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect( hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	UINT createDeviceFlags = 0;

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

	D3D_FEATURE_LEVEL FeatureLevel;

	if( FAILED (hr = D3D11CreateDeviceAndSwapChain( NULL, 
		D3D_DRIVER_TYPE_HARDWARE,
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

	//for debug create a texture
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

	m_pTexture = NULL;
	hr = D3DX11CreateTextureFromFile(m_pd3dDevice, "Media\\test.bmp", &loadInfo, NULL, &m_pTexture, NULL);
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
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_TEXTURE2D_DESC desc;
	ID3D11Texture2D *pTexture2D = (ID3D11Texture2D*)m_pTexture;
	pTexture2D->GetDesc( &desc );

	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = desc.MipLevels -1;

	ID3D11ShaderResourceView *pSRView = NULL;
	m_pd3dDevice->CreateShaderResourceView( m_pTexture, &srvDesc, &pSRView );

}

void ELRenderer::Shutdown()
{
	//Delete m_VertexBuffers
	for(int i=0; i<ELRenderer_Max_Vertex_Buffers; i++)
		if(m_VertexBuffers[i] != 0)
			DeleteVertexBuffer(i);

	//Delete m_IndexBuffers
	for(int i=0; i<ELRenderer_Max_Index_Buffers; i++)
		if(m_IndexBuffers[i] != 0)
			DeleteIndexBuffer(i);

	//Delete m_GeometryShaderVarsBuffer
	m_GeometryShaderVarsBuffer->Release();

	//Delete Geometry VShader
	m_GeometryVShader->Release();

	//Delete Geometry PShader
	m_GeometryPShader->Release();

	//Delete m_GeometryLayout
	m_GeometryLayout->Release();
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

	//for Pix debug
	UINT shaderFlags = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	hr = D3DX11CompileFromFile(ELRenderer_GeometryPShader_FilePath, NULL, NULL, ELRenderer_GeometryPShader_Func, "ps_4_0", shaderFlags, 0, NULL, &pshader, &pErrmsg, NULL);
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

void ELRenderer::DrawMesh(const int IBuffer, const int VBuffer, int NumTriangles)
{
	UINT stride = sizeof(float) * 6;
	UINT offset = 0;

	m_pd3dDeviceContext->IASetVertexBuffers( 0, 1, &m_VertexBuffers[VBuffer], &stride, &offset );
	m_pd3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	m_pd3dDeviceContext->IASetIndexBuffer(m_IndexBuffers[IBuffer], DXGI_FORMAT_R16_UINT, 0);

	m_pd3dDeviceContext->DrawIndexed(NumTriangles*3, 0, 0);
}

void ELRenderer::BeginGeometryDebug()
{
	m_pd3dDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView_Screen, pDSV );
	float ClearColor[4] = { 0.0f, 0.05f, 0.5f, 1.0f }; //red,green,blue,alpha
	m_pd3dDeviceContext->ClearRenderTargetView( m_pRenderTargetView_Screen, ClearColor );
	m_pd3dDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_pd3dDeviceContext->VSSetShader(m_GeometryVShader, NULL, 0);
	m_pd3dDeviceContext->IASetInputLayout(m_GeometryLayout);

	m_pd3dDeviceContext->PSSetShader(m_GeometryPShader, NULL, 0);

	m_pd3dDeviceContext->RSSetViewports( 1, &m_vp );

}

void ELRenderer::EndGeometryDebug()
{
	m_pSwapChain->Present( 0, 0 );
}
