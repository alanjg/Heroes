#include "stdafx.h"
#include "Camera.h"
#include "Renderer.h"


Camera::Camera(Renderer& renderer)
	:m_renderer(renderer)
{
	m_viewConstantBuffer = NULL;
}

Camera::~Camera()
{
	if (m_viewConstantBuffer) m_viewConstantBuffer->Release();
}

void Camera::Initialize()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(ViewConstantBuffer);
	HRESULT hr = m_renderer.g_pd3dDevice->CreateBuffer(&bd, NULL, &m_viewConstantBuffer);
	assert(SUCCEEDED(hr));

	m_atX = 0;
	m_atY = 0;
	m_atZ = 0;
	m_eyeX = 0;
	m_eyeY = 15;
	m_eyeZ = -30;
	LookAt();
}

void Camera::LookAt()
{
	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(m_eyeX, m_eyeY, m_eyeZ, 0.0f);
	XMVECTOR At = XMVectorSet(m_atX, m_atY, m_atZ, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);
	XMStoreFloat4x4(&m_viewMatrix, view);

	ViewConstantBuffer viewConstantBuffer;
	viewConstantBuffer.mView = XMMatrixTranspose(view);
	m_renderer.g_pImmediateContext->UpdateSubresource(m_viewConstantBuffer, 0, NULL, &viewConstantBuffer, 0, 0);
}

void Camera::SetPosition(float x, float y, float z)
{
	m_eyeX = x;
	m_eyeY = y;
	m_eyeZ = z;
}

void Camera::Look(float eyeX, float eyeY, float eyeZ)
{
	m_atX = eyeX;
	m_atY = eyeY;
	m_atZ = eyeZ;
}

void Camera::Move(float deltaX, float deltaY, float deltaZ)
{
	m_eyeX += deltaX;
	m_eyeY += deltaY;
	m_eyeZ += deltaZ;

	m_atX += deltaX;
	m_atY += deltaY;
	m_atZ += deltaZ;
}

void Camera::Zoom(float delta)
{
	float dx, dy, dz;
	dx = m_atX - m_eyeX;
	dy = m_atY - m_eyeY;
	dz = m_atZ - m_eyeZ;
	float mag = sqrt(dx*dx + dy*dy + dz*dz);
	dx /= mag;
	dy /= mag;
	dz /= mag;

	float newMag = mag + delta;
	if (newMag < 1)
	{
		newMag = 1;
	}
	if (newMag > 50)
	{
		newMag = 50;
	}

	m_eyeX = m_atX - dx * newMag;
	m_eyeY = m_atY - dy * newMag;
	m_eyeZ = m_atZ - dz * newMag;
}

ID3D11Buffer** Camera::GetViewTransformBuffer()
{
	return &m_viewConstantBuffer;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return m_viewMatrix;
}