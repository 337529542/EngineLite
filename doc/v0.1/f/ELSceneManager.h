#pragma once
#include "ELEntity.h"
#include "ELRenderer.h"
#include <vector>

class ELSceneManager
{
public:
	ELSceneManager();

	std::vector<ELEntity> mEntityList;

	ELEntity mTestEnt;

	ELRenderer renderer;

	void renderOneFrame();
	void Setup();
	void setViewMatrix(ELMatrix4x4 *viewMatrix);
	void setPerspectiveMatrix(ELMatrix4x4 *perspectiveMatrix);

private:
	int vs;//vshader index
	int ps;//pshader index

	ELMatrix4x4 m_viewMatrix;
	ELMatrix4x4 m_perspectiveMatrix;

};