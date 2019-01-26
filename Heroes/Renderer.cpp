#include "stdafx.h"
#include "Renderer.h"
#include "SkinnedMesh.h"
#include "StaticMesh.h"
#include "SkinnedMeshInstance.h"
#include "StaticMeshInstance.h"
#include "Entity.h"
#include "Camera.h"
#include "File.h"
#include "Collision.h"
#include <DDSTextureLoader.h>

using namespace DirectX;
Renderer::Renderer(HINSTANCE hinst, HWND hwnd)
{
	g_hInst = hinst;
	g_hWnd = hwnd;
	g_driverType = D3D_DRIVER_TYPE_NULL;
	g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	g_pd3dDevice = NULL;
	g_pImmediateContext = NULL;
	g_pSwapChain = NULL;
	g_pRenderTargetView = NULL;
	g_pDepthStencil = NULL;
	g_pDepthStencilView = NULL;
	g_pSkinnedVertexShaderModel = NULL;
	g_pSkinnedPixelShaderModel = NULL;
	g_pSkinnedVertexLayoutModel = NULL;

	g_pStaticVertexShaderModel = NULL;
	g_pStaticPixelShaderModel = NULL;
	g_pStaticVertexLayoutModel = NULL;
	g_pBlendState = NULL;
	m_projectionConstantBuffer = NULL;
	g_pCBSkinnedModelMatrices = NULL;
	g_pTextureRV = NULL;
	g_pSamplerLinear = NULL;
	m_pD2DFactory = NULL;
	m_pRT = NULL;
	m_pTextBrush = NULL;
	m_pSelectionBrush = NULL;
	m_pDWriteFactory = NULL;
	m_pTextFormat = NULL;
}

HRESULT Renderer::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
		{
			OutputDebugStringA((char*) pErrorBlob->GetBufferPointer());
			MessageBoxA(NULL, (char*) pErrorBlob->GetBufferPointer(), "Error", MB_OK);
		}
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT Renderer::InitShaders(WCHAR* file, ID3D11VertexShader** vertexShader, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements, ID3D11InputLayout** inputLayout, ID3D11PixelShader** pixelShader)
{
	HRESULT hr;
	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;

	hr = CompileShaderFromFile(file, "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, vertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), inputLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	//g_pImmediateContext->IASetInputLayout(*inputLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(file, "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, pixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	return hr;
}

void LoadMesh(Renderer* renderer, const std::vector<std::string> directories, std::vector<SkinnedMesh*>& meshes)
{

	int limit = 30;
	int count = 0;

	for each(auto s in directories)
	{
		if (limit == count) break;
		SkinnedMesh* mesh = new SkinnedMesh(renderer);
		HRESULT hr = mesh->CreateModelGeometry(s);
		if (FAILED(hr))
		{
			continue;
		}
		meshes.push_back(mesh);
		count++;
	}
}

HRESULT Renderer::InitDevice()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	// thanks windows 10
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes [] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels [] =
	{
//		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;
		

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	SetViewport(width, height);

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC skinnedModelLayout [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "MATRIXINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = InitShaders(L"SkinnedMesh.fx", &g_pSkinnedVertexShaderModel, skinnedModelLayout, ARRAYSIZE(skinnedModelLayout), &g_pSkinnedVertexLayoutModel, &g_pSkinnedPixelShaderModel);
	if (FAILED(hr)) return hr;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC staticModelLayout [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = InitShaders(L"StaticMesh.fx", &g_pStaticVertexShaderModel, staticModelLayout, ARRAYSIZE(staticModelLayout), &g_pStaticVertexLayoutModel, &g_pStaticPixelShaderModel);
	if (FAILED(hr)) return hr;

	
	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_camera = std::shared_ptr<Camera>(new Camera(*this));
	m_camera->Initialize();
	// Create the constant buffers
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(ProjectionConstantBuffer);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &m_projectionConstantBuffer);
	assert(SUCCEEDED(hr));

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr))
		return hr;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.RenderTarget[0] = rtbd;

	hr = g_pd3dDevice->CreateBlendState(&blendDesc, &g_pBlendState);
	if (FAILED(hr))
		return hr;

	// Initialize the projection matrix
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (float) height, 0.01f, 1000.0f);
	XMStoreFloat4x4(&g_Projection, projection);

	ProjectionConstantBuffer projectionConstantBuffer;
	projectionConstantBuffer.mProjection = XMMatrixTranspose(projection);
	g_pImmediateContext->UpdateSubresource(m_projectionConstantBuffer, 0, NULL, &projectionConstantBuffer, 0, 0);


	// D2D and DWrite
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &m_pD2DFactory);
	if (FAILED(hr)) return hr;

	IDXGISurface *dxgiBackbuffer;
	hr = g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackbuffer));
	if (FAILED(hr)) return hr;
	
	DXGI_SURFACE_DESC desc;
	hr = dxgiBackbuffer->GetDesc(&desc);
	if (FAILED(hr)) return hr;

	//get the dpi information
	HDC screen = GetDC(0);
	m_dpiScaleX = GetDeviceCaps(screen, LOGPIXELSX) / 96.0f;
	m_dpiScaleY = GetDeviceCaps(screen, LOGPIXELSY) / 96.0f;
	ReleaseDC(0, screen);

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(dxgiBackbuffer, props, &m_pRT);
	if (FAILED(hr)) return hr;
	//dxgiBackbuffer->Release();

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	if (FAILED(hr)) return hr;
	hr = m_pDWriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"en-us", &m_pTextFormat);
	if (FAILED(hr)) return hr;
	//hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	//if (FAILED(hr)) return hr;
	//hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	//if (FAILED(hr)) return hr;

	//D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	//hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(g_hWnd, size), &m_pRT);
	//if (FAILED(hr)) return hr;
	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
	if (FAILED(hr)) return hr;

	hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pSelectionBrush);
	if (FAILED(hr)) return hr;



	return S_OK;
}


/******************************************************************
*                                                                 *
*  SimpleText::DrawText                                           *
*                                                                 *
*  This method will draw text using the IDWriteTextFormat         *
*  via the Direct2D render target                                 *
*                                                                 *
******************************************************************/

HRESULT Renderer::DrawText(const std::wstring& text)
{
	HRESULT hr = S_OK;
	m_pRT->BeginDraw();

	m_pRT->SetTransform(D2D1::IdentityMatrix());

	RECT rc;

	GetClientRect(g_hWnd, &rc);

	// Create a D2D rect that is the same size as the window.

	D2D1_RECT_F layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.left) / m_dpiScaleX,
		static_cast<FLOAT>(rc.top) / m_dpiScaleY,
		static_cast<FLOAT>(rc.right - rc.left) / m_dpiScaleX,
		static_cast<FLOAT>(rc.bottom - rc.top) / m_dpiScaleY
		);


	// Use the DrawText method of the D2D render target interface to draw.
	
	m_pRT->DrawText(text.c_str(), text.size(), m_pTextFormat, layoutRect, m_pTextBrush);
	hr = m_pRT->EndDraw();
	return S_OK;
}

HRESULT Renderer::DrawRect(int left, int top, int right, int bottom)
{
	HRESULT hr = S_OK;
	m_pRT->BeginDraw();

	m_pRT->SetTransform(D2D1::IdentityMatrix());

	RECT rc;

	GetClientRect(g_hWnd, &rc);

	// Create a D2D rect that is the same size as the window.

	D2D1_RECT_F layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.left) / m_dpiScaleX,
		static_cast<FLOAT>(rc.top) / m_dpiScaleY,
		static_cast<FLOAT>(rc.right - rc.left) / m_dpiScaleX,
		static_cast<FLOAT>(rc.bottom - rc.top) / m_dpiScaleY
		);


	// Use the DrawText method of the D2D render target interface to draw.
	D2D1_RECT_F rect = D2D1::RectF((float)left, (float)top, (float)right, (float)bottom);
	m_pRT->DrawRectangle(rect, m_pSelectionBrush);
	
	hr = m_pRT->EndDraw();
	return S_OK;
}

	
void Renderer::SetViewport(int width, int height)
{
	m_viewportWidth = width;
	m_viewportHeight = height;
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT) width;
	vp.Height = (FLOAT) height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);
}

Renderer::~Renderer()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pTextureRV) g_pTextureRV->Release();
	if (m_projectionConstantBuffer) m_projectionConstantBuffer->Release();
	if (g_pCBSkinnedModelMatrices) g_pCBSkinnedModelMatrices->Release();
	if (g_pSkinnedVertexLayoutModel) g_pSkinnedVertexLayoutModel->Release();
	if (g_pSkinnedVertexShaderModel) g_pSkinnedVertexShaderModel->Release();
	if (g_pSkinnedPixelShaderModel) g_pSkinnedPixelShaderModel->Release();
	if (g_pStaticVertexLayoutModel) g_pStaticVertexLayoutModel->Release();
	if (g_pStaticVertexShaderModel) g_pStaticVertexShaderModel->Release();
	if (g_pStaticPixelShaderModel) g_pStaticPixelShaderModel->Release();

	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();

	if (m_pD2DFactory) m_pD2DFactory->Release();
	if (m_pRT) m_pRT->Release();
	if (m_pDWriteFactory) m_pDWriteFactory->Release();
	if (m_pTextBrush) m_pTextBrush->Release();
	if (m_pTextFormat) m_pTextFormat->Release();
	if (m_pSelectionBrush) m_pSelectionBrush->Release();
}

void Renderer::BeginRenderPass()
{
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	float blendFactor [] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_pImmediateContext->OMSetBlendState(g_pBlendState, blendFactor, 0xffffffff);

	g_pImmediateContext->VSSetConstantBuffers(0, 1, &m_projectionConstantBuffer);
	m_rendererdTriangleCount = 0;
}

void Renderer::CompleteRenderPass()
{
	g_pSwapChain->Present(0, 0);
}

SkinnedMeshInstance* Renderer::CreateSkinnedMeshInstance(const std::string& meshName)
{
	auto meshIt = skinnedMeshLookup.find(meshName);
	if (meshIt == skinnedMeshLookup.end())
	{
		LoadModel(modelDirectory + meshName);
	}
	meshIt = skinnedMeshLookup.find(meshName);
	if (meshIt != skinnedMeshLookup.end())
	{
		std::shared_ptr<SkinnedMeshInstance> instance(new SkinnedMeshInstance(this));
		instance->BindMesh(meshIt->second);
		skinnedMeshInstances.push_back(instance);
		return instance.get();
	}
	return nullptr;
}

StaticMeshInstance* Renderer::CreateStaticMeshInstance(const std::string& meshName)
{
	auto meshIt = staticMeshLookup.find(meshName);
	if (meshIt == staticMeshLookup.end())
	{
		LoadModel(modelDirectory + meshName);
	}
	meshIt = staticMeshLookup.find(meshName);
	if (meshIt != staticMeshLookup.end())
	{
		std::shared_ptr<StaticMeshInstance> instance(new StaticMeshInstance(this));
		instance->BindMesh(meshIt->second);
		staticMeshInstances.push_back(instance);
		return instance.get();
	}
	return nullptr;
}


	//int offsetX = 0, offsetY = 0;
	//for each(MeshInstance* mesh in meshInstances)
	//{
	//	XMMATRIX world;
	//	if (true)
	//	{
	//		if (true)
	//		{
	//			for (int i = 0; i < 30; i++)
	//			{
	//				for (int j = 0; j < 20; j++)
	//				{
	//					// Rotate model around the origin
	//					float cameraRotation = rotateCamera ? t : 0;

	//					world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(cameraRotation / 5 + XM_PIDIV4 * 3.0f) * XMMatrixTranslation(-0.137041f, -1.8f, -1.06089f) * XMMatrixTranslation(4 * i - 15 + j, -2, 4 * j + 5);
	//					XMFLOAT4X4 w;
	//					XMStoreFloat4x4(&w, world);
	//					mesh->SetTransform(w);
	//					mesh->RenderModel(animationIndex, t);
	//				}
	//			}
	//		}
	//		else
	//		{
	//			world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(t / 5 + XM_PIDIV4 * 3.0f) * XMMatrixTranslation(-0.137041f, -1.8f, -1.06089f) * XMMatrixTranslation(0, 0, 0);
	//			XMFLOAT4X4 w;
	//			XMStoreFloat4x4(&w, world);
	//			mesh->SetTransform(w);
	//			mesh->RenderModel(animationIndex, t);
	//		}
	//	}
	//	else
	//	{		
	//		world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(3 * XM_PIDIV4) * XMMatrixTranslation(-0.137041f, -1.8f, -1.06089f) * XMMatrixTranslation(-12+3*(offsetX), 0, 5*offsetY);
	//		XMFLOAT4X4 w;
	//		XMStoreFloat4x4(&w, world);
	//		mesh->SetTransform(w);
	//		mesh->RenderModel(animationIndex, t);
	//	}

	//	if (writeModel)
	//	{
	//		writeModel = false;
	//		mesh->WriteGeometry(&world);
	//	}
	//	offsetX++;
	//	if (offsetX == 12)
	//	{
	//		offsetX = 0;
	//		offsetY++;
	//	}
	//}

ID3D11Buffer** Renderer::GetViewTransformBuffer()
{
	return m_camera->GetViewTransformBuffer();
}

ID3D11Buffer** Renderer::GetProjectionTransformBuffer()
{
	return &m_projectionConstantBuffer;
}

std::shared_ptr<Camera> Renderer::GetCamera()
{
	return m_camera;
}

bool Renderer::ProjectPickingRayToPlane(int x, int y, CXMVECTOR planePoint, CXMVECTOR planeNormal, XMVECTOR& intersectionPoint)
{
	XMMATRIX P = XMLoadFloat4x4(&g_Projection);

	// Compute picking ray in view space.
	float vx = (+2.0f*x / m_viewportWidth - 1.0f) / P.r[0].m128_f32[0];
	float vy = (-2.0f*y / m_viewportHeight + 1.0f) / P.r[1].m128_f32[1];

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = XMLoadFloat4x4(&m_camera->GetViewMatrix());
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);


	rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
	rayDir = XMVector3TransformNormal(rayDir, invView);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	float dist = 0.0f;
	return XNA::IntersectRayPlane(rayOrigin, rayDir, planePoint, planeNormal, intersectionPoint, &dist);
}

Entity* Renderer::Pick(int sx, int sy)
{	
	XMMATRIX P = XMLoadFloat4x4(&g_Projection);

	// Compute picking ray in view space.
	float vx = (+2.0f*sx / m_viewportWidth - 1.0f) / P.r[0].m128_f32[0];
	float vy = (-2.0f*sy / m_viewportHeight + 1.0f) / P.r[1].m128_f32[1];

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = XMLoadFloat4x4(&m_camera->GetViewMatrix());
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	for (auto m : skinnedMeshInstances)
	{
		XMMATRIX W = XMLoadFloat4x4(&m->GetTransform());
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		XMVECTOR transformedRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		XMVECTOR transformedRayDir = XMVector3TransformNormal(rayDir, toLocal);

		// Make the ray direction unit length for the intersection tests.
		transformedRayDir = XMVector3Normalize(transformedRayDir);

		// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
		// so do the ray/triangle tests.
		//
		// If we did not hit the bounding box, then it is impossible that we hit 
		// the Mesh, so do not waste effort doing ray/triangle tests.

		float tmin = 0.0f;
		if (m->Pick(transformedRayOrigin, transformedRayDir, tmin))
		{
			return m->GetEntity();
		}
	}

	for (auto m : staticMeshInstances)
	{
		XMMATRIX W = XMLoadFloat4x4(&m->GetTransform());
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		XMVECTOR transformedRayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		XMVECTOR transformedRayDir = XMVector3TransformNormal(rayDir, toLocal);

		// Make the ray direction unit length for the intersection tests.
		transformedRayDir = XMVector3Normalize(transformedRayDir);

		// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
		// so do the ray/triangle tests.
		//
		// If we did not hit the bounding box, then it is impossible that we hit 
		// the Mesh, so do not waste effort doing ray/triangle tests.

		float tmin = 0.0f;
		if (m->Pick(transformedRayOrigin, transformedRayDir, tmin))
		{
			return m->GetEntity();
		}
	}

	return nullptr;
}

void Renderer::Pick(int sx, int sy, int sw, int sh, std::vector<Entity*>& hitElements)
{
	XMMATRIX P = XMLoadFloat4x4(&g_Projection);
	XMMATRIX V = XMLoadFloat4x4(&m_camera->GetViewMatrix());

	for each(auto m in skinnedMeshInstances)
	{
		if (m->GetEntity() != nullptr)
		{
			XMMATRIX W = XMLoadFloat4x4(&m->GetTransform());
			XMFLOAT4 coord;
			coord.x = 0;
			coord.y = 0;
			coord.z = 0;
			coord.w = 1;
			XMVECTOR cv = XMLoadFloat4(&coord);
			XMVECTOR screen = XMVector3Project(cv, 0, 0, (float)m_viewportWidth, (float)m_viewportHeight, 0.01f, 1000.0f, P, V, W);
			XMFLOAT4 screenCoord;
			XMStoreFloat4(&screenCoord, screen);
			if (sx <= screenCoord.x && sx + sw >= screenCoord.x && sy < screenCoord.y && sy + sh >= screenCoord.y)
			{
				hitElements.push_back(m->GetEntity());
			}
		}
	}

	for each(auto m in staticMeshInstances)
	{
		if (m->GetEntity() != nullptr)
		{
			XMMATRIX W = XMLoadFloat4x4(&m->GetTransform());
			XMFLOAT4 coord;
			coord.x = 0;
			coord.y = 0;
			coord.z = 0;
			coord.w = 1;
			XMVECTOR cv = XMLoadFloat4(&coord);
			XMVECTOR screen = XMVector3Project(cv, 0, 0, (float)m_viewportWidth, (float)m_viewportHeight, 0.01f, 1000.0f, P, V, W);
			XMFLOAT4 screenCoord;
			XMStoreFloat4(&screenCoord, screen);
			if (sx <= screenCoord.x && sx + sw >= screenCoord.x && sy < screenCoord.y && sy + sh >= screenCoord.y)
			{
				hitElements.push_back(m->GetEntity());
			}
		}
	}
}

std::string Renderer::LoadModel(const std::string& directory)
{
	std::string meshName = directory;
	if (meshName[meshName.size() - 1] == '/' || meshName[meshName.size() - 1] == '\\')
	{
		meshName.pop_back();
	}
	int lastBackslash = meshName.find_last_of('\\');
	int lastSlash = meshName.find_last_of('/');
	int last = std::max(lastBackslash, lastSlash);
	if (last != -1)
	{
		meshName = meshName.substr(last + 1);
	}
	struct stat buffer;
	std::string meshFile = directory + "\\Geometry.skinnedmesh.smd.bin";

	if (stat(meshFile.c_str(), &buffer) == 0)
	{
		SkinnedMesh* mesh = new SkinnedMesh(this);
		std::shared_ptr<SkinnedMesh> meshPtr(mesh);
		HRESULT hr = mesh->CreateModelGeometry(directory);
		if (FAILED(hr))
		{
			return "";
		}
		skinnedMeshLookup[meshName] = mesh;
		skinnedModelMeshes.push_back(meshPtr);
	}
	else
	{
		StaticMesh* mesh = new StaticMesh(this);
		std::shared_ptr<StaticMesh> meshPtr(mesh);
		HRESULT hr = mesh->CreateModelGeometry(directory);
		if (FAILED(hr))
		{
			return "";
		}
		staticMeshLookup[meshName] = mesh;
		staticModelMeshes.push_back(meshPtr);
	}
	return meshName;
}

void Renderer::LoadModels(const std::string& directory)
{
	std::vector<std::string> directories = GetDirectoriesInDirectory(directory);
	for each(auto s in directories)
	{
		LoadModel(s);
	}
}

void Renderer::RegisterModelDirectory(const std::string& directory)
{
	modelDirectory = directory;
}

const std::unordered_map<std::string, SkinnedMesh*>& Renderer::GetSkinnedMeshLookup()
{
	return skinnedMeshLookup;
}

const std::unordered_map<std::string, StaticMesh*>& Renderer::GetStaticMeshLookup()
{
	return staticMeshLookup;
}