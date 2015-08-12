#pragma once
#include "VertexDefinitions.h"
#include "MeshInstance.h"

class StaticMesh;
class Renderer;

class StaticMeshInstance : public MeshInstance
{
private:
	StaticMesh* m_mesh;
public:
	StaticMeshInstance(Renderer* renderer);
	HRESULT BindMesh(StaticMesh* mesh);
	virtual void RenderModel();
	virtual void WriteGeometry(XMMATRIX* transform);
	virtual bool Pick(XMVECTOR rayOrigin, XMVECTOR rayDirection, float& tMin);
};