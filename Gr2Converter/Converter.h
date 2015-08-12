#pragma once

using namespace DirectX;
using namespace DirectX::PackedVector;

bool ConvertSMD(std::string source, std::string dest);

struct SkinnedMeshVertex
{
	XMFLOAT3 v;
	XMFLOAT4 weights;
	XMBYTE4 MatrixIndices;
	XMFLOAT3 n;
	XMFLOAT2 Tex;
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
	std::string material;
	SkinnedModelVertex v[3];
};

struct StaticMeshVertex
{
	XMFLOAT3 v;
	XMFLOAT3 n;
	XMFLOAT2 Tex;
};