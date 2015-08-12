#pragma once

class Renderer;
using namespace DirectX;

struct ViewConstantBuffer
{
	XMMATRIX mView;
};


class Camera
{
	ID3D11Buffer* m_viewConstantBuffer = NULL;
	XMFLOAT4X4 m_viewMatrix;
	Renderer& m_renderer;
	float m_eyeX, m_eyeY, m_eyeZ;
	float m_atX, m_atY, m_atZ;
public:
	Camera(Renderer& renderer);
	~Camera();

	void Initialize();
	void LookAt();
	void SetPosition(float x, float y, float z);
	void Look(float eyeX, float eyeY, float eyeZ);
	void Move(float deltaX, float deltaY, float deltaZ);
	void Zoom(float delta);
	
	ID3D11Buffer** GetViewTransformBuffer();
	XMFLOAT4X4 GetViewMatrix();
};

