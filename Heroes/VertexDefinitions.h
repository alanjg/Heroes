#pragma once

using namespace DirectX;
using namespace DirectX::PackedVector;

struct SkinnedMeshVertex
{
	XMFLOAT3 v;
	XMFLOAT4 weights;
	XMBYTE4 MatrixIndices;
	XMFLOAT3 n;
	XMFLOAT2 Tex;
};

// also referenced in SkinnedMesh.fx
const int MAX_BONES = 184;

struct CBSkinnedMatrices
{
	XMFLOAT4X4 matrices[MAX_BONES];
};

// Used for StaticMesh.fx
struct StaticMeshVertex
{
	XMFLOAT3 v;
	XMFLOAT3 n;
	XMFLOAT2 Tex;
};