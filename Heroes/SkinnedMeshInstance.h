#pragma once
#include "VertexDefinitions.h"
#include "MeshInstance.h"

class SkinnedMesh;
class Renderer;

class SkinnedMeshInstance : public MeshInstance
{
private:
	SkinnedMesh* m_mesh;
	ID3D11Buffer* m_skinnedModelMatrices;
	CBSkinnedMatrices m_boneMatricesTransformed; // world pose
	std::vector<XMFLOAT4X4> m_accumulatedBoneTransforms;
	int m_activeAnimation;
	float m_activeAnimationTime;
	bool m_cycleActiveAnimation;
public:
	SkinnedMeshInstance(Renderer* renderer);
	HRESULT BindMesh(SkinnedMesh* mesh);

	virtual void RenderModel();
	void SetAnimation(int animation, float t, bool cycle=true);
	void SetAnimation(const std::string& animation, float t, bool cycle=true);
	void PlayAnimationSound(int animation);
	void PlayAnimationSound(const std::string& animation);
	
	virtual void WriteGeometry(XMMATRIX* transform);
	virtual bool Pick(XMVECTOR rayOrigin, XMVECTOR rayDirection, float& tMin);
};