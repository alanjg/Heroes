#pragma once
#include "VertexDefinitions.h"
#include "Renderer.h"
#include "Spline.h"

class SkinnedMeshInstance;

struct StaticMeshPart
{
	std::vector<StaticMeshVertex> triangles;
	std::string material;
	ID3D11Buffer* vertexBuffer;
	ID3D11ShaderResourceView* texture;
};

class StaticMesh
{
	friend class StaticMeshInstance;

	Renderer* renderer;
	std::string directoryRoot;
	std::vector<StaticMeshPart> meshParts;

	HRESULT LoadBinaryMesh(std::istream& in, const std::string& directoryRoot);
	ID3D11ShaderResourceView* LoadTexture(StaticMeshPart& meshPart, const std::string& directoryRoot, const std::string& material);
	ID3D11Buffer* LoadVertexBuffer2(StaticMeshVertex* vertices, int vertexCount);
public:
	StaticMesh(Renderer* renderer);
	~StaticMesh();
	HRESULT CreateModelGeometry(const std::string& directoryRoot);
	void RenderModel(ID3D11Buffer* worldTransformBuffer);
};