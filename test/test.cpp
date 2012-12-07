#include "ELRenderer.h"

#include <stdlib.h>
#include <windows.h>


HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL; 


ELRenderer *renderer;
/////////////////////////////////////////////////////////////////////////////////////


void TestStart()
{
	renderer = new ELRenderer;
	renderer->Setup(g_hWnd);
}

void TestEnd()
{
	renderer->Shutdown();
	delete renderer;
}

void TestUpdate()
{

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