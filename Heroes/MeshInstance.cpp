#include "stdafx.h"
#include "MeshInstance.h"

MeshInstance::MeshInstance(Renderer* renderer)
{
	m_worldTransformConstantBuffer = NULL;
	m_renderer = renderer;
	XMStoreFloat4x4(&m_transform, XMMatrixIdentity());
}

MeshInstance::~MeshInstance()
{
	if (m_worldTransformConstantBuffer) m_worldTransformConstantBuffer->Release();
}


void MeshInstance::SetTransform(const XMFLOAT4X4& transform)
{
	m_transform = transform;
}


const XMFLOAT4X4& MeshInstance::GetTransform()
{
	return m_transform;
}

void MeshInstance::BindEntity(Entity* entity)
{
	m_entity = entity;
}

Entity* MeshInstance::GetEntity()
{
	return m_entity;
}
