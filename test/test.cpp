#include "ELRenderer.h"

#include <stdlib.h>
#include <windows.h>

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

}

void TestEnd()
{
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

	renderer->BeginGeometryDebug();

	//test SetGeoShaderVars
	//if(renderer->SetGeometryConstant(&GeoShaderVars) == -1)
	//	throw "foo";

	renderer->EndGeometryDebug();
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