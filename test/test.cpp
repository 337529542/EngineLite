#include "ELRenderer.h"

#include <stdlib.h>
#include <windows.h>

#include "ELCube.h"
#include "ELMath.h"

#ifdef _DEBUG
void Trace(LPCTSTR lpszFmt, ...)
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
#define TRACE(fmt, sz) Trace(fmt, sz)
#else
#define TRACE(fmt, sz)
#endif


//for debug view
int fps = 0;
int fps_c = 0;
DWORD TickCount = 0;
//////////////////


HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL; 


ELRenderer *renderer;
/////////////////////////////////////////////////////////////////////////////////////

float testVBuffer[]={1.0f, 1.0f, 1.0f,
					2.0f, 2.0f, 2.0f,
					3.0f, 3.0f, 3.0f,
					};
int VBufferHandles[567];

unsigned short testIBuffer[]={1, 2, 3, 4, 5, 6, 7, 8, 9};
int IBufferHandles[789];

ELRenderer_ShaderVars_Geometry GeoShaderVars;

ELCube *cube1;

int tex2D[100];

void TestStart()
{
	renderer = new ELRenderer;
	renderer->Setup(g_hWnd);

	//test VBuffers
	for(int i=0; i<567; i++)
	{
		VBufferHandles[i] = renderer->CreateVertexBuffer(testVBuffer, 9);
		if(VBufferHandles[i] == -1)
			throw "foo";
	}

	//testIBuffers
	for(int i=0; i<789; i++)
	{
		IBufferHandles[i] = renderer->CreateIndexBuffer(testIBuffer, 9);
		if(IBufferHandles[i] == -1)
			throw "foo";
	}

	//test load texture
	for(int i=0; i<1; i++)
	{
		tex2D[i] = renderer->CreateTexture2D("Media\\AmmoBoxdiffus.png");
		if(tex2D[i] == -1)
			TRACE("%s", "create tex failed\n");
	}

	//add a cube
	cube1 = new ELCube(renderer);
}

void TestEnd()
{
	delete cube1;
	renderer->Shutdown();
	delete renderer;
}

void TestUpdate()
{
	//debug show fps
	if(TickCount == 0)
	{
		TickCount = GetTickCount();
	}
	fps_c ++;
	DWORD nowt = GetTickCount();
	if(nowt >= TickCount + 1000)
	{
		TickCount = nowt;
		fps = fps_c;
		fps_c = 0;
		TRACE("fps:%d\n", fps);
	}
	//
	ELMatrix4x4 ViewMatrix;
	ELMatrix4x4 PerspectiveMatrix;

	PerspectiveMatrix.makePerspectiveMatrix(D3DX_PI * 0.4f, 800.0f/600.0f, 0.1f, 1000.0f);
	ViewMatrix.resetMatrix();
	ViewMatrix.setTrans(ELVector3(0, 0, 70));
	cube1->GetWorldMatrix()->resetMatrix();

	static float yaw = 0;
	ELMatrix4x4 Smat4;
	Smat4.setScale(ELVector3(0.5f, 0.5f, 0.5f));
	cube1->GetWorldMatrix()->setYaw(yaw);
	yaw += 0.0005;

	cube1->GetWorldMatrix()->mul(Smat4);

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			GeoShaderVars.viewMatrix[i][j] = ViewMatrix.m[i][j];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			GeoShaderVars.worldMatrix[i][j] = cube1->GetWorldMatrix()->m[i][j];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			GeoShaderVars.perspectiveMatrix[i][j] = PerspectiveMatrix.m[i][j];

	renderer->BeginGeometryDebug();

	if(renderer->SetGeometryConstant(&GeoShaderVars) == -1)
		throw "foo";

	renderer->SetGeometryDiffuseTexture2D(tex2D[0]);

	cube1->DrawMesh();

	renderer->EndGeometryDebug();

	renderer->BeginLightingDebug();
	renderer->EndLightingDebug();
}
/////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch( message )
	{
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}


HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "EngineLiteTest";
	wcex.hIconSm = NULL;
	if( !RegisterClassEx( &wcex ) )
		return E_FAIL;
	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = CreateWindow( "EngineLiteTest", "Test", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL );
	if( !g_hWnd )
		return E_FAIL;
	ShowWindow( g_hWnd, nCmdShow );
	return S_OK;
}


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

	TestStart();

	// Main message loop
	MSG msg = {0};
	while( WM_QUIT != msg.message )
	{
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		//Do render here
		TestUpdate();
	}

	TestEnd();

	return ( int )msg.wParam;
}