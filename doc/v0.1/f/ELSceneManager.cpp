#include "ELSceneManager.h"
#include "ELLog.h"
#include <stdio.h>
#include <iostream>

char vshader[10000];
char pshader[10000];


ELSceneManager::ELSceneManager()
{
	//
	mEntityList.clear();
}

void ELSceneManager::renderOneFrame()
{
	renderer.setRenderStage(0);

	renderer.setVarViewMatrix(&m_viewMatrix);
	renderer.setPerspectiveMatrix(&m_perspectiveMatrix);

	renderer.begin();
	renderer.setVShader(vs);
	renderer.setPShader(ps);
	renderer.drawEntity(&mTestEnt);
	renderer.end();
}

void ELSceneManager::Setup()
{
	//add one vshader and one pshader

	vs = renderer.addVShader("../../Media/SimpleVertexShader.hlsl", "SimpleVertexShader");

	if(vs != -1)
		ELLog::getInstance()<<"add vshader success.vs index is :"<<vs<<endl;
	else
		ELLog::getInstance()<<"add vshader failed"<<endl;

	ps = renderer.addPShader("../../Media/SimplePixelShader.hlsl", "SimplePixelShader");

	if(ps != -1)
		ELLog::getInstance()<<"add pshader success.ps index is :"<<ps<<endl;
	else
		ELLog::getInstance()<<"add pshader failed"<<endl;
}

void ELSceneManager::setViewMatrix( ELMatrix4x4 *viewMatrix )
{
	m_viewMatrix = *viewMatrix;
}

void ELSceneManager::setPerspectiveMatrix( ELMatrix4x4 *perspectiveMatrix )
{
	m_perspectiveMatrix = *perspectiveMatrix;
}