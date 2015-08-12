#include "stdafx.h"
#include "StaticMesh.h"
#include "Renderer.h"
#include "Camera.h"
#include "File.h"
using namespace std;

StaticMesh::StaticMesh(Renderer* r) : renderer(r)
{
}

StaticMesh::~StaticMesh()
{
}

HRESULT StaticMesh::LoadBinaryMesh(istream& infile, const std::string& directoryRoot)
{
	StaticMesh& mesh = *this;
	
	string temp;

	int materialCount;
	infile.read((char*) &materialCount, sizeof(materialCount));

	meshParts.resize(materialCount);
	for (int i = 0; i < materialCount; i++)
	{
		StaticMeshPart& meshPart = meshParts[i];
		int materialLen;
		infile.read((char*) &materialLen, sizeof(materialLen));
		string material;
		material.resize(materialLen);
		infile.read(&material[0], sizeof(char)*materialLen);

		meshPart.texture = LoadTexture(meshPart, directoryRoot, material);
		meshPart.material = material;

		int triangleCount;
		infile.read((char*) &triangleCount, sizeof(triangleCount));
		meshPart.triangles.resize(triangleCount * 3);

		infile.read((char*) &meshPart.triangles[0], sizeof(StaticMeshVertex)*triangleCount * 3);

		meshPart.vertexBuffer = LoadVertexBuffer2(&meshPart.triangles[0], meshPart.triangles.size());
	}

	if (!infile) return E_FAIL;
	return S_OK;
}

HRESULT StaticMesh::CreateModelGeometry(const std::string& directoryRoot)
{
	StaticMesh& mesh = *this;
	mesh.directoryRoot = directoryRoot;

	bool useBinaryMesh = true;
	struct stat buffer;
	string binaryMeshFile = directoryRoot + "\\Geometry.staticmesh.smd.bin";

	if (useBinaryMesh && stat(binaryMeshFile.c_str(), &buffer) == 0)
	{
		ifstream infile(binaryMeshFile, ios::binary);

		HRESULT hr = LoadBinaryMesh(infile, directoryRoot);
		if (FAILED(hr)) return hr;
	}
	else
	{
		string meshFile = directoryRoot + "\\Geometry.staticmesh.smd";
		ifstream infile(meshFile);

		//HRESULT hr = LoadMesh(infile, directoryRoot);
		//if (FAILED(hr)) return hr;
		return E_FAIL;
	}

	return S_OK;
}

ID3D11ShaderResourceView* StaticMesh::LoadTexture(StaticMeshPart& meshPart, const std::string& directoryRoot, const std::string& material)
{
	ID3D11ShaderResourceView* texture;

	string textureFile = directoryRoot + "\\" + material + ".tga.dds";
	struct stat buffer;
	if (stat(textureFile.c_str(), &buffer) != 0)
	{
		textureFile = directoryRoot + "\\" + material + ".dds";
		if (stat(textureFile.c_str(), &buffer) != 0)
		{
			textureFile = directoryRoot + "\\" + material + ".(Texture).dds";
			if (stat(textureFile.c_str(), &buffer) != 0)
			{
				textureFile = directoryRoot + "\\" + material + ".1.(Texture).dds";
				if (stat(textureFile.c_str(), &buffer) != 0)
				{
					bool bad = true;
				}
			}
		}
	}

	wstring txf(textureFile.begin(), textureFile.end());
	HRESULT hr = CreateDDSTextureFromFile(renderer->g_pd3dDevice, txf.c_str(), NULL, &texture);
	if (FAILED(hr)) return NULL;
	return texture;
}

ID3D11Buffer* StaticMesh::LoadVertexBuffer2(StaticMeshVertex* vertices, int vertexCount)
{
	ID3D11Buffer* pBuffer;
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(StaticMeshVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = renderer->g_pd3dDevice->CreateBuffer(&bd, &InitData, &pBuffer);
	if (FAILED(hr))
		return NULL;

	// Set vertex buffer
	UINT stride = sizeof(StaticMeshVertex);
	UINT offset = 0;
	renderer->g_pImmediateContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);

	return pBuffer;
}

void StaticMesh::RenderModel(ID3D11Buffer* worldTransformBuffer)
{
	// Set vertex buffer
	renderer->g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride2 = sizeof(StaticMeshVertex);
	UINT offset2 = 0;

	for (unsigned int i = 0; i < meshParts.size(); i++)
	{
		renderer->g_pImmediateContext->IASetInputLayout(renderer->g_pStaticVertexLayoutModel);
		renderer->g_pImmediateContext->IASetVertexBuffers(0, 1, &meshParts[i].vertexBuffer, &stride2, &offset2);
		renderer->g_pImmediateContext->VSSetShader(renderer->g_pStaticVertexShaderModel, NULL, 0);
		renderer->g_pImmediateContext->VSSetConstantBuffers(0, 1, renderer->GetViewTransformBuffer());
		renderer->g_pImmediateContext->VSSetConstantBuffers(1, 1, renderer->GetProjectionTransformBuffer());
		renderer->g_pImmediateContext->VSSetConstantBuffers(2, 1, &worldTransformBuffer);
		renderer->g_pImmediateContext->PSSetShader(renderer->g_pStaticPixelShaderModel, NULL, 0);
		renderer->g_pImmediateContext->PSSetConstantBuffers(2, 1, &worldTransformBuffer);
		renderer->g_pImmediateContext->PSSetShaderResources(0, 1, &meshParts[i].texture);
		renderer->g_pImmediateContext->PSSetSamplers(0, 1, &renderer->g_pSamplerLinear);
		renderer->g_pImmediateContext->Draw(meshParts[i].triangles.size(), 0);

		renderer->m_rendererdTriangleCount += meshParts[i].triangles.size();
	}
}