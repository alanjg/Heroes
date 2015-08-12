#pragma once
#include "VertexDefinitions.h"

class Renderer;
class Entity;

class MeshInstance
{
protected:
	Renderer* m_renderer;
	Entity* m_entity;
	ID3D11Buffer* m_worldTransformConstantBuffer;
	XMFLOAT4X4 m_transform;
public:
	MeshInstance(Renderer* renderer);
	virtual ~MeshInstance();
	void BindEntity(Entity* entity);
	Entity* GetEntity();

	virtual void RenderModel() = 0;

	void SetTransform(const XMFLOAT4X4& transform);
	const XMFLOAT4X4& GetTransform();
	virtual void WriteGeometry(XMMATRIX* transform) = 0;
	virtual bool Pick(XMVECTOR rayOrigin, XMVECTOR rayDirection, float& tMin) = 0;
};