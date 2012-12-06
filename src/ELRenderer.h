#pragma once

class ELRenderer
{
public:
	ELRenderer();
	~ELRenderer();

	void CreateMesh();//vertex buffer, index buffer, normal buffer, texcoord buffer...
	void DeleteMesh();

	void BeginGeometry();

		void SetConstant();
		void DrawMesh();

	void EndGeometryAndBeginLighting();

	void EndLightingAndBeginComposition();

	void EndConposition();
};