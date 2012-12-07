#include "ELRenderer.h"
#include "ELLog.h"

#include <D3DX11async.h>
#include <d3d10shader.h>


ELRenderer::ELRenderer()
:m_renderStage(0)
{
	for(int i=0; i<MAX_VSHADERS; i++)
		m_VShader[i] = NULL;

	for(int i=0; i<MAX_PSHADERS; i++)
		m_PShader[i] = NULL;
}

ELRenderer::~ELRenderer()
{

}

void ELRenderer::setup( HWND hWnd )
{
	ELLog::getInstance()<<"Setup Renderer"<<endl;
	HRESULT hr = S_OK;;
	RECT rc;
	GetClientRect( hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	UINT createDeviceFlags = 0;


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
		ELLog::getInstance()<<"Error Setup Renderer,with error id:"<<GetLastError()<<endl;
		return;
	}


	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) )
	{
		ELLog::getInstance()<<"Error Create render target, GetBuffer()"<<endl;
		return;
	}
	hr = m_pd3dDevice->CreateRenderTargetView( pBackBuffer,NULL, &m_pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) )
	{
		ELLog::getInstance()<<"Error Create render target, CreateRenderTargetView()"<<endl;
		return;
	}

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pd3dDeviceContext->RSSetViewports( 1, &vp );

	//set up vars in shaders
	D3D11_BUFFER_DESC viewMatrixBD;
	viewMatrixBD.Usage = D3D11_USAGE_DYNAMIC; 
	viewMatrixBD.ByteWidth = sizeof(ELRenderer_ShaderVars); 
	viewMatrixBD.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	viewMatrixBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	viewMatrixBD.MiscFlags = 0; 
	viewMatrixBD.StructureByteStride = 0;

	hr = m_pd3dDevice->CreateBuffer(&viewMatrixBD, NULL, &m_D3DViewMatrixBuffer); 
	if(FAILED(hr)) 
	{ 
		ELLog::getInstance()<<"Error Create View Matrix Buffer"<<endl;
		return; 
	}

	ELLog::getInstance()<<"Create View Matrix Buffer OK"<<endl;

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
		ELLog::getInstance()<<"Error Create pDepthStencil Buffer"<<endl;
		return; 
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
		ELLog::getInstance()<<"Error Create CreateDepthStencilState"<<endl;
		return; 
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
		ELLog::getInstance()<<"Error Create CreateDepthStencilView"<<endl;
		return; 
	}

	// Bind the depth stencil view
	//m_pd3dDeviceContext->OMSetRenderTargets( 1,          // One rendertarget view
	//	&m_pRenderTargetView,      // Render target view, created earlier
	//	pDSV );     // Depth stencil view for the render target


	return; 
}

void ELRenderer::setRenderStage( int s )
{
	m_renderStage = s;
}

void ELRenderer::begin()
{
	if(m_renderStage == 0)
	{
		m_pd3dDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, pDSV );
		float ClearColor[4] = { 0.0f, 0.05f, 0.5f, 1.0f }; //red,green,blue,alpha
		m_pd3dDeviceContext->ClearRenderTargetView( m_pRenderTargetView, ClearColor );
		m_pd3dDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	else if(m_renderStage == 1)
	{
		;
	}
}

void ELRenderer::end()
{
	if(m_renderStage == 0)
	{
		m_pSwapChain->Present( 0, 0 );
	}
	else if(m_renderStage == 1)
	{
		;
	}
}

int ELRenderer::addVShader( char* shader, char* funcName )
{
	int index = -1;
	for(int i=0; i<MAX_VSHADERS; i++)
	{
		if(m_VShader[i] == NULL)
		{
			HRESULT hr;

			//compile shader
			ID3D10Blob *pshader;
			ID3D10Blob *pErrmsg;
			hr = D3DX11CompileFromFile(shader, NULL, NULL, funcName, "vs_4_0", 0, 0, NULL, &pshader, &pErrmsg, NULL);
			if( FAILED(hr) )
			{
				m_VShader[i] = NULL;
				ELLog::getInstance()<<"VShader Complie failed "<<(char*)pErrmsg->GetBufferPointer()<<endl;
			}
			else
			{
				//create vshader
				hr = m_pd3dDevice->CreateVertexShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), NULL, &(m_VShader[i]));
				if( FAILED(hr) )
				{
					m_VShader[i] = NULL;
					ELLog::getInstance()<<"VShader add failed with code : "<<(unsigned int)hr<<endl;
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
						&(m_InputLayout[i])
						);

					if( FAILED(hr) )
					{
						ELLog::getInstance()<<"VShader Layout create failed with code : "<<(unsigned int)hr<<endl;
					}


					index = i;
				}
			}

			break;
		}
	}

	return index;
}

int ELRenderer::addPShader( char* shader, char* funcName )
{
	int index = -1;
	for(int i=0; i<MAX_PSHADERS; i++)
	{
		if(m_PShader[i] == NULL)
		{
			HRESULT hr;

			//compile shader
			ID3D10Blob *pshader;
			ID3D10Blob *pErrmsg;
			//hr = D3DX11CompileFromFile(shader, NULL, NULL, funcName, "ps_4_0", D3D10_SHADER_SKIP_OPTIMIZATION, 0, NULL, &pshader, &pErrmsg, NULL);
			hr = D3DX11CompileFromFile(shader, NULL, NULL, funcName, "ps_4_0", 0, 0, NULL, &pshader, &pErrmsg, NULL);
			if( FAILED(hr) )
			{
				m_PShader[i] = NULL;
				ELLog::getInstance()<<"PShader Complie failed "<<(char*)pErrmsg->GetBufferPointer()<<endl;
			}
			else
			{
				//create pshader
				hr = m_pd3dDevice->CreatePixelShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), NULL, &(m_PShader[i]));
				if( FAILED(hr) )
				{
					m_PShader[i] = NULL;
					ELLog::getInstance()<<"PShader add failed with code : "<<(unsigned int)hr<<endl;
				}
				else
				{
					index = i;
				}
			}

			break;
		}
	}

	return index;
}

void ELRenderer::setVShader( int index )
{
	m_pd3dDeviceContext->VSSetShader(m_VShader[index], NULL, 0);
	m_pd3dDeviceContext->IASetInputLayout(m_InputLayout[index]);
}

void ELRenderer::setPShader( int index )
{
	m_pd3dDeviceContext->PSSetShader(m_PShader[index], NULL, 0);
}

void ELRenderer::drawEntity( ELEntity *ent )
{
	ID3D11Buffer *pVBuffer;
	ID3D11Buffer *pIBuffer;
	HRESULT hr;

	//update variables
	ELRenderer_ShaderVars *shaderVarsMapPtr;

	D3D11_MAPPED_SUBRESOURCE viewMatrixMappedResource;
	hr = m_pd3dDeviceContext->Map(m_D3DViewMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &viewMatrixMappedResource); 
	if(FAILED(hr)) 
	{ 
		ELLog::getInstance()<<ELLog::getInstance().GetTimeStr()<<"viewMatrixMappedResource failed "<<endl;
		return;
	}

	shaderVarsMapPtr = (ELRenderer_ShaderVars *)viewMatrixMappedResource.pData;

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			shaderVarsMapPtr->viewMatrix[i][j] = m_viewMatrix.m[j][i];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			shaderVarsMapPtr->worldMatrix[i][j] = ent->worldMatrix.m[j][i];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			shaderVarsMapPtr->perspectiveMatrix[i][j] = m_perspectiveMatrix.m[j][i];

	m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_D3DViewMatrixBuffer);

	m_pd3dDeviceContext->Unmap(m_D3DViewMatrixBuffer, 0);
	

	//create vertex buffer
	/*float vertices[] = 
	{
		-0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( float ) * 3 * 3 ;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;*/

	//create vertex buffer from entity
	float *p_veritices = new float[ent->mesh.getNumTriangles()*18];

	//position data
	int j = 0;
	for(std::vector<ELTriangle>::iterator i=ent->mesh.triPoint.begin(); i!=ent->mesh.triPoint.end(); i++)
	{
		p_veritices[j*18 + 0] = i->v0.x;
		p_veritices[j*18 + 1] = i->v0.y;
		p_veritices[j*18 + 2] = i->v0.z;

		p_veritices[j*18 + 6] = i->v1.x;
		p_veritices[j*18 + 7] = i->v1.y;
		p_veritices[j*18 + 8] = i->v1.z;

		p_veritices[j*18 + 12] = i->v2.x;
		p_veritices[j*18 + 13] = i->v2.y;
		p_veritices[j*18 + 14] = i->v2.z;

		//debug
		/*ELLog::getInstance()<<*(p_veritices+j*9 + 0)<<" "<<*(p_veritices+j*9 + 1)<<" "<<*(p_veritices+j*9 + 2)<<endl;
		ELLog::getInstance()<<p_veritices[j*9 + 3]<<" "<<p_veritices[j*9 + 4]<<" "<<p_veritices[j*9 + 5]<<endl;
		ELLog::getInstance()<<p_veritices[j*9 + 6]<<" "<<p_veritices[j*9 + 7]<<" "<<p_veritices[j*9 + 8]<<endl;
		ELLog::getInstance()<<ent->mesh.getNumTriangles()<<endl;*/
		/////////////////////////////////

		j++;
	}

	//normal data
	j = 0;
	for(std::vector<ELTriangle>::iterator i=ent->mesh.triNormal.begin(); i!=ent->mesh.triNormal.end(); i++)
	{
		p_veritices[j*18 + 3] = i->v0.x;
		p_veritices[j*18 + 4] = i->v0.y;
		p_veritices[j*18 + 5] = i->v0.z;

		p_veritices[j*18 + 9] = i->v1.x;
		p_veritices[j*18 + 10] = i->v1.y;
		p_veritices[j*18 + 11] = i->v1.z;

		p_veritices[j*18 + 15] = i->v2.x;
		p_veritices[j*18 + 16] = i->v2.y;
		p_veritices[j*18 + 17] = i->v2.z;

		j++;
	}

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( float ) * ent->mesh.getNumTriangles()*18 ;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = p_veritices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;



	hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &pVBuffer);
	if( FAILED(hr) )
	{
		ELLog::getInstance()<<ELLog::getInstance().GetTimeStr()<<"Create VBuffer failed "<<endl;
	}
	else
	{
		ELLog::getInstance()<<ELLog::getInstance().GetTimeStr()<<"Create VBuffer ok "<<endl;
	}

	//set vertex buffer
	UINT stride = sizeof(float) * 6;
	UINT offset = 0;

	m_pd3dDeviceContext->IASetVertexBuffers( 0, 1, &pVBuffer, &stride, &offset );
	m_pd3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	////create index buffer
	//unsigned short indices[] = 
	//{
	//	0, 1, 2, 2, 1, 0,
	//};

	//D3D11_BUFFER_DESC indexBufferDesc;
	//indexBufferDesc.ByteWidth = sizeof(unsigned short) * 6;
	//indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufferDesc.CPUAccessFlags = 0;
	//indexBufferDesc.MiscFlags = 0;
	//indexBufferDesc.StructureByteStride = 0;

	//D3D11_SUBRESOURCE_DATA indexBufferData;
	//indexBufferData.pSysMem = indices;
	//indexBufferData.SysMemPitch = 0;
	//indexBufferData.SysMemSlicePitch = 0;

	//create index buffer from entity
	unsigned short *p_indices = new unsigned short [ent->mesh.getNumTriangles()*3];
	for(int i=0; i<ent->mesh.getNumTriangles()*3; i++)
	{
		p_indices[i] = i;
		//ELLog::getInstance()<<p_indices[i]<<endl;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * ent->mesh.getNumTriangles()*3;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = p_indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	hr = m_pd3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &pIBuffer);
	if( FAILED(hr) )
	{
		ELLog::getInstance()<<ELLog::getInstance().GetTimeStr()<<"Create IBuffer failed "<<endl;
	}
	else
	{
		ELLog::getInstance()<<ELLog::getInstance().GetTimeStr()<<"Create IBuffer ok "<<endl;
	}

	//set index buffer
	m_pd3dDeviceContext->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R16_UINT, 0);

	//draw
	m_pd3dDeviceContext->DrawIndexed(ent->mesh.getNumTriangles()*3, 0, 0);
	//m_pd3dDeviceContext->DrawIndexed(2*3, 0, 0);

	//delete vertex buffer
	pVBuffer->Release();
	pIBuffer->Release();

	delete[] p_veritices;
	delete[] p_indices;

}

void ELRenderer::setVarViewMatrix(ELMatrix4x4 *viewMatrix)
{
	m_viewMatrix = *viewMatrix;
}

void ELRenderer::setPerspectiveMatrix( ELMatrix4x4 *perspectiveMatrix )
{
	m_perspectiveMatrix = *perspectiveMatrix;
}