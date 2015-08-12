#include "stdafx.h"
#include "Entity.h"
#include "MeshInstance.h"

Entity::Entity()
{	
}

Entity::~Entity()
{
}

void Entity::Render()
{
	m_mesh->RenderModel();
}

void Entity::Initialize(float xx, float yy, MeshInstance* m)
{
	x = xx;
	y = yy;
	m_mesh = m;
	orientation = 0;
}

void Entity::Update(float elapsedTime)
{
	XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(orientation + XM_PIDIV2) * XMMatrixTranslation(-0.137041f, -1.8f, -1.06089f) * XMMatrixTranslation(x, 0, y);
	XMFLOAT4X4 w;
	XMStoreFloat4x4(&w, world);
	m_mesh->SetTransform(w);
}