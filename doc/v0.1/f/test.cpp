#include "ELEntity.h"
#include "ELMath.h"
#include "ELDaeMeshLoader.h"
#include "ELLog.h"
#include "ELRoot.h"

#include <stdlib.h>
#include <windows.h>

void dump_matrix(ELMatrix4x4 m)
{
	printf("\n");

	printf("     |%f   %f    %f    %f|\n", m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]);
	printf("     |%f   %f    %f    %f|\n", m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3]);
	printf("     |%f   %f    %f    %f|\n", m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3]);
	printf("     |%f   %f    %f    %f|\n", m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]);

	printf("\n\n\n");
}

int main()
{
	//ELMatrix4x4 m, m1;
	//m.resetMatrix();
	//m1.resetMatrix();
	////m.setScale(ELVector3(2, 3, 4));
	////m.setPitch(2.0);

	//for(int i=0; i<4; i++)
	//	for(int j=0; j<4; j++)
	//	{
	//		m.m[i][j] = i*4 + j;
	//		m1.m[i][j]= 100 + i*4 + j;
	//	}

	//m.mul(m1);

	//dump_matrix(m);

	//load mesh
	/*ELEntity ent;
	if(ELDaeMeshLoader::loadMeshFromFile("../../Media/fangzi.DAE", ent.mesh))
	{
		printf("Load Mesh OK!, numTriangles : %d\n", ent.mesh.getNumTriangles());
	}
	else
	{
		printf("Load Mesh Failed!\n");
		system("pause");
		return 0;
	}*/

	//ELLog mainLog;
	//mainLog.CommonLogInit();
	//mainLog<<mainLog.GetTimeStr()<<"ok."<<endl;

	ELRoot *pRoot = new ELRoot;



	/*ELEntity ent;
	if(ELDaeMeshLoader::loadMeshFromFile("../../Media/fangzi.DAE", ent.mesh))
	{
		printf("Load Mesh OK!, numTriangles : %d\n", ent.mesh.getNumTriangles());
		pRoot->m_smgr.mEntityList.push_back(ent);
	}
	else
	{
		printf("Load Mesh Failed!\n");
	}


	delete pRoot;*/

	ELMatrix4x4 m;
	m.makePerspectiveMatrix(D3DX_PI * 0.7f, 800.0f/600.0f, 1.0f, 100.0f);
	dump_matrix(m);
	

	system("pause");

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL; 


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
	// Register class注册窗口
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;//LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "EngineLiteTest";
	wcex.hIconSm = NULL;//LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	if( !RegisterClassEx( &wcex ) )
		return E_FAIL;
	// Create window创建窗口
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


ELMatrix4x4 viewMat;
ELMatrix4x4 perspectiveMat;
float scalee = 0.5;
float yaw = 0;
void createScene(ELRoot *pRoot)
{
	viewMat.resetMatrix();
	perspectiveMat.resetMatrix();
	perspectiveMat.makePerspectiveMatrix(D3DX_PI * 0.5f, 800.0f/600.0f, 1.0f, 1000.0f);
	pRoot->m_smgr.setPerspectiveMatrix(&perspectiveMat);


	//load entity
	if(ELDaeMeshLoader::loadMeshFromFile("../../Media/fangzi.DAE", pRoot->m_smgr.mTestEnt.mesh))
	{
		//printf("Load Mesh OK!, numTriangles : %d\n", pRoot->m_smgr.mTestEnt.mesh.getNumTriangles());
		ELLog::getInstance()<<"Load Mesh OK!, numTriangles : "<<pRoot->m_smgr.mTestEnt.mesh.getNumTriangles()<<endl;
	}
	else
	{
		printf("Load Mesh Failed!\n");
	}
}

void Render(ELRoot *pRoot)
{
	scalee += 0.02;
	yaw += 0.05;

	if(scalee > 1)
	{
		scalee = 0.5;
	}

	viewMat.resetMatrix();
	viewMat.setTrans(ELVector3(0, 0, 100.0f));
	pRoot->m_smgr.setViewMatrix(&viewMat);

	pRoot->m_smgr.mTestEnt.worldMatrix.resetMatrix();
	pRoot->m_smgr.mTestEnt.worldMatrix.setYaw(yaw);
	
	pRoot->m_smgr.renderOneFrame();
}


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;


	ELRoot *pRoot = new ELRoot;

	pRoot->m_smgr.renderer.setup(g_hWnd);
	pRoot->m_smgr.Setup();

	createScene(pRoot);

	// Main message loop主要的消息循环
	MSG msg = {0};
	while( WM_QUIT != msg.message )
	{
		/*if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			pRoot->m_smgr.renderOneFrame();
		}*/
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		Render(pRoot);
	}


	delete pRoot;

	return ( int )msg.wParam;
}