#pragma once
#include "VertexDefinitions.h"
#include "Renderer.h"
#include "Spline.h"

class SkinnedMeshInstance;
struct SkinnedModelBone
{
	int id;
	int parentId;
	std::string name;
};

struct SkinnedModelVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	std::vector<int> linkIds;
	std::vector<float> linkWeights;
	double globalWeight;
	int globalLink;
};

struct SkinnedModelTriangle
{
	SkinnedModelVertex v[3];
};

struct ScaleShearKeyframes
{
	std::vector<XMFLOAT4X4> keyframes;
};

struct OrientationKeyframes
{
	std::vector<XMFLOAT4> keyframes;
};

struct PositionKeyframes
{
	std::vector<XMFLOAT4> keyframes;
};

enum AnimationType
{
	AnimationNone,
	AnimationIdentity,
	AnimationConstant,
	AnimationSpline,
	AnimationKeyframes
};

struct BoneAnimation
{
	BoneAnimation()
	{
		positionAnimationType = AnimationNone;
		orientationAnimationType = AnimationNone;
		scaleShearAnimationType = AnimationNone;
	}
	AnimationType positionAnimationType;
	XMFLOAT4* positionConstant;
	PositionKeyframes* positionKeyframes;
	Spline* positionSpline;

	AnimationType orientationAnimationType;
	XMFLOAT4* orientationConstant;
	OrientationKeyframes* orientationKeyframes;
	Spline* orientationSpline;

	AnimationType scaleShearAnimationType;
	XMFLOAT4X4* scaleShearConstant;
	ScaleShearKeyframes* scaleShearKeyframes;
	Spline* scaleShearSpline;
};

struct Animation
{
	std::string name;
	float duration;

	// indexed by bone
	std::vector<BoneAnimation> boneAnimations;

	std::string soundFile;
};

struct Bone
{
	static Bone Identity()
	{
		Bone b;
		XMStoreFloat4(&b.position, XMVectorZero());
		XMStoreFloat4(&b.orientation, XMQuaternionIdentity());
		XMStoreFloat4x4(&b.scaleShear, XMMatrixIdentity());
		XMStoreFloat4x4(&b.matrix, XMMatrixIdentity());
		XMStoreFloat4x4(&b.inverseWorld, XMMatrixIdentity());
		return b;
	}
	Bone()
	{
	}
	XMFLOAT4 position;
	XMFLOAT4 orientation;
	XMFLOAT4X4 scaleShear;
	XMFLOAT4X4 matrix;
	XMFLOAT4X4 inverseWorld;
};

struct MeshPart
{
	std::vector<SkinnedMeshVertex> triangles;
	std::string material;
	ID3D11Buffer* vertexBuffer;
	ID3D11ShaderResourceView* texture;
};

class SkinnedMesh
{
	friend class SkinnedMeshInstance;

	Renderer* renderer;
	std::string directoryRoot;
	std::vector<Bone> boneList;
	std::vector<Animation> animations;
	std::vector<SkinnedModelBone> bones;
	std::vector<MeshPart> meshParts;
	
	void LoadBinaryAnimation(Animation& animation, std::istream& in, std::vector<SkinnedModelBone>& bones);
	void LoadAnimation(Animation& animation, std::istream& anim, std::vector<SkinnedModelBone>& bones);
	void LoadBinarySkeleton(std::istream& skel, std::vector<SkinnedModelBone>& bones);
	void LoadSkeleton(std::istream& skel, std::vector<SkinnedModelBone>& bones);
	HRESULT LoadMesh(std::istream& in, const std::string& directoryRoot);
	HRESULT LoadBinaryMesh(std::istream& in, const std::string& directoryRoot);
	ID3D11ShaderResourceView* LoadTexture(MeshPart& meshPart, const std::string& directoryRoot, const std::string& material);
	ID3D11Buffer* LoadVertexBuffer2(SkinnedMeshVertex* vertices, int vertexCount);
public:
	SkinnedMesh(Renderer* renderer);
	~SkinnedMesh();
	HRESULT CreateModelGeometry(const std::string& directoryRoot);
	HRESULT LoadVertexBuffer(std::vector<SkinnedModelTriangle>& triangles, MeshPart& meshPart);
	void RenderModel(int animation, float t, bool cycle, std::vector<XMFLOAT4X4>& accumulatedBoneTransforms, ID3D11Buffer* skinnedModelMatrices, ID3D11Buffer* worldTransformBuffer, CBSkinnedMatrices* boneMatricesTransformed);

};