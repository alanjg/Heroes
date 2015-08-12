#include "stdafx.h"
#include "StaticMeshInstance.h"
#include "StaticMesh.h"
#include "Renderer.h"
#include "Camera.h"
#include "Collision.h"
using namespace std;

StaticMeshInstance::StaticMeshInstance(Renderer* renderer)
	: MeshInstance(renderer)
{
}

HRESULT StaticMeshInstance::BindMesh(StaticMesh* mesh)
{
	m_mesh = mesh;

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(WorldTransformConstantBuffer);
	HRESULT hr = m_renderer->g_pd3dDevice->CreateBuffer(&bd, NULL, &m_worldTransformConstantBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void StaticMeshInstance::RenderModel()
{
	WorldTransformConstantBuffer cb;
	XMMATRIX transform = XMLoadFloat4x4(&m_transform);
	cb.mWorld = XMMatrixTranspose(transform);
	m_renderer->g_pImmediateContext->UpdateSubresource(m_worldTransformConstantBuffer, 0, NULL, &cb, 0, 0);

	m_mesh->RenderModel(m_worldTransformConstantBuffer);
}

void StaticMeshInstance::WriteGeometry(XMMATRIX* transform)
{
	ofstream outfile("model.wrl");
	outfile << "#VRML V2.0 utf8" << endl;
	outfile << endl;
	outfile << "Shape{" << endl;
	outfile << "geometry DEF Mesh01_part01 IndexedFaceSet{" << endl;
	outfile << "coord Coordinate{" << endl;
	outfile << "point[" << endl;

	int triangleCount = 0;
	for each(auto p in m_mesh->meshParts)
	{
		for each(StaticMeshVertex v in p.triangles)
		{
			XMVECTOR pos;
			pos.m128_f32[0] = v.v.x;
			pos.m128_f32[1] = v.v.y;
			pos.m128_f32[2] = v.v.z;
			pos.m128_f32[3] = 1.0;
			
			pos = XMVector3Transform(pos, *transform);
			XMFLOAT4X4 viewMatrix = m_mesh->renderer->GetCamera()->GetViewMatrix();
			XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
			pos = XMVector3Transform(pos, view);
			outfile << "\t\t\t" << pos.m128_f32[0] << "\t" << pos.m128_f32[1] << "\t" << pos.m128_f32[2] << "," << endl;
		}
	}
	outfile << "]" << endl;
	outfile << "}" << endl;
	outfile << "coordIndex [" << endl;
	for (int i = 0; i < triangleCount; i++)
	{
		outfile << (i * 3 + 0) << ", " << (i * 3 + 1) << ", " << (i * 3 + 2) << ", -1" << endl;
	}
	outfile << "]" << endl;
	outfile << "}" << endl;
	outfile << "}" << endl;
}

bool StaticMeshInstance::Pick(XMVECTOR rayOrigin, XMVECTOR rayDirection, float& tMin)
{
	// Assume we have not picked anything yet, so init to -1.
	int pickedTriangle = -1;
	float tmin = std::numeric_limits<float>::max();
	XNA::AxisAlignedBox meshBox;
	meshBox.Center = XMFLOAT3(0, 0, 0);
	meshBox.Extents = XMFLOAT3(2, 2, 2);
	float tHit = tMin;
	if (XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDirection, &meshBox, &tHit))
	{
		for each(auto p in m_mesh->meshParts)
		{
			for (unsigned int i = 0; i < p.triangles.size(); i += 3)
			{
				XMVECTOR vFinal[3];
				for (int j = 0; j < 3; j++)
				{
					StaticMeshVertex v = p.triangles[i + j];
					vFinal[j].m128_f32[0] = v.v.x;
					vFinal[j].m128_f32[1] = v.v.y;
					vFinal[j].m128_f32[2] = v.v.z;
					vFinal[j].m128_f32[3] = 1.0;
				}
				float t = 0.0f;
				if (XNA::IntersectRayTriangle(rayOrigin, rayDirection, vFinal[0], vFinal[1], vFinal[2], &t))
				{
					if (t < tmin)
					{
						// This is the new nearest picked triangle.
						tmin = t;
						pickedTriangle = i;
					}
				}
			}
		}
	}
	if (pickedTriangle != -1)
	{
		return true;
	}
	return false;
}
