#include "stdafx.h"
#include "SkinnedMeshInstance.h"
#include "SkinnedMesh.h"
#include "Renderer.h"
#include "Camera.h"
#include "Collision.h"
using namespace std;

SkinnedMeshInstance::SkinnedMeshInstance(Renderer* renderer)
	:MeshInstance(renderer)
{
	m_worldTransformConstantBuffer = NULL;
	m_renderer = renderer;
	XMStoreFloat4x4(&m_transform, XMMatrixIdentity());
}

HRESULT SkinnedMeshInstance::BindMesh(SkinnedMesh* mesh)
{
	m_mesh = mesh;

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());
	m_accumulatedBoneTransforms.resize(mesh->boneList.size(), identity);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(CBSkinnedMatrices);
	HRESULT hr = m_renderer->g_pd3dDevice->CreateBuffer(&bd, NULL, &m_skinnedModelMatrices);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(WorldTransformConstantBuffer);
	hr = m_renderer->g_pd3dDevice->CreateBuffer(&bd, NULL, &m_worldTransformConstantBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void SkinnedMeshInstance::RenderModel()
{
	WorldTransformConstantBuffer cb;
	XMMATRIX transform = XMLoadFloat4x4(&m_transform);
	cb.mWorld = XMMatrixTranspose(transform);
	m_renderer->g_pImmediateContext->UpdateSubresource(m_worldTransformConstantBuffer, 0, NULL, &cb, 0, 0);

	m_mesh->RenderModel(m_activeAnimation, m_activeAnimationTime, m_accumulatedBoneTransforms, m_skinnedModelMatrices, m_worldTransformConstantBuffer, &m_boneMatricesTransformed);
}

void SkinnedMeshInstance::SetAnimation(int animation, float t)
{
	m_activeAnimation = animation;
	m_activeAnimationTime = t;
}

void SkinnedMeshInstance::SetAnimation(std::string animation, float t)
{
	int index = 0;
	for each(const Animation& a in m_mesh->animations)
	{
		if (a.name == animation)
		{
			SetAnimation(index, t);
		}
		index++;
	}
}

void SkinnedMeshInstance::WriteGeometry(XMMATRIX* transform)
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
		for each(SkinnedMeshVertex v in p.triangles)
		{
			XMVECTOR pos, acc;
			acc = XMVectorZero();
			pos.m128_f32[0] = v.v.x;
			pos.m128_f32[1] = v.v.y;
			pos.m128_f32[2] = v.v.z;
			pos.m128_f32[3] = 1.0;

			int indices[4];
			indices[0] = v.MatrixIndices.x;
			indices[1] = v.MatrixIndices.y;
			indices[2] = v.MatrixIndices.z;
			indices[3] = v.MatrixIndices.w;
			float weights[4];
			weights[0] = v.weights.x;
			weights[1] = v.weights.y;
			weights[2] = v.weights.z;
			weights[3] = v.weights.w;
			for (unsigned int j = 0; j < 4; j++)
			{
				int boneIndex = indices[j];
				XMMATRIX bone = XMLoadFloat4x4(&m_boneMatricesTransformed.matrices[boneIndex]);
				bone = XMMatrixTranspose(bone);
				XMVECTOR w = XMVector3Transform(pos, bone);
				acc += w * weights[j];
			}
			acc = XMVector3Transform(acc, *transform);
			XMFLOAT4X4 viewMatrix = m_mesh->renderer->GetCamera()->GetViewMatrix();
			XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
			acc = XMVector3Transform(acc, view);
			outfile << "\t\t\t" << acc.m128_f32[0] << "\t" << acc.m128_f32[1] << "\t" << acc.m128_f32[2] << "," << endl;
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

bool SkinnedMeshInstance::Pick(XMVECTOR rayOrigin, XMVECTOR rayDirection, float& tMin)
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
					SkinnedMeshVertex v = p.triangles[i + j];
					XMVECTOR pos, acc;
					acc = XMVectorZero();
					pos.m128_f32[0] = v.v.x;
					pos.m128_f32[1] = v.v.y;
					pos.m128_f32[2] = v.v.z;
					pos.m128_f32[3] = 1.0;

					int indices[4];
					indices[0] = v.MatrixIndices.x;
					indices[1] = v.MatrixIndices.y;
					indices[2] = v.MatrixIndices.z;
					indices[3] = v.MatrixIndices.w;
					float weights[4];
					weights[0] = v.weights.x;
					weights[1] = v.weights.y;
					weights[2] = v.weights.z;
					weights[3] = v.weights.w;
					for (unsigned int k = 0; k < 4; k++)
					{
						int boneIndex = indices[k];
						XMMATRIX bone = XMLoadFloat4x4(&m_boneMatricesTransformed.matrices[boneIndex]);
						bone = XMMatrixTranspose(bone);
						XMVECTOR w = XMVector3Transform(pos, bone);
						acc += w * weights[k];
					}
					vFinal[j] = acc;
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
