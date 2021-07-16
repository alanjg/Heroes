#pragma once

class SkinnedMesh;
class StaticMesh;
class SkinnedMeshInstance;
class StaticMeshInstance;
class Entity;
class Camera;

using namespace DirectX;

struct ProjectionConstantBuffer
{
	XMMATRIX mProjection;
};

struct WorldTransformConstantBuffer
{
	XMMATRIX mWorld;
};

class Renderer
{
	ID3D11Buffer* m_projectionConstantBuffer;
	std::shared_ptr<Camera> m_camera;
	int m_viewportWidth, m_viewportHeight;

	std::unordered_map<std::string, SkinnedMesh*> skinnedMeshLookup;
	std::vector<std::shared_ptr<SkinnedMesh>> skinnedModelMeshes;
	std::vector<std::shared_ptr<SkinnedMeshInstance>> skinnedMeshInstances;


	std::unordered_map<std::string, StaticMesh*> staticMeshLookup;
	std::vector<std::shared_ptr<StaticMesh>> staticModelMeshes;
	std::vector<std::shared_ptr<StaticMeshInstance>> staticMeshInstances;
	std::string modelDirectory;
public:

	int64_t m_rendererdTriangleCount;
	HINSTANCE                           g_hInst;
	HWND                                g_hWnd;

	// D2D variables
	ID2D1Factory* m_pD2DFactory;
	ID2D1RenderTarget* m_pRT;
	ID2D1SolidColorBrush* m_pTextBrush;
	ID2D1SolidColorBrush* m_pSelectionBrush;

	// DirectWrite
	IDWriteFactory1* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	float m_dpiScaleX;
	float m_dpiScaleY;

	// D3D variables
	D3D_DRIVER_TYPE                     g_driverType;
	D3D_FEATURE_LEVEL                   g_featureLevel;
	ID3D11Device*                       g_pd3dDevice;
	ID3D11DeviceContext*                g_pImmediateContext;
	IDXGISwapChain*                     g_pSwapChain;
	ID3D11RenderTargetView*             g_pRenderTargetView;
	ID3D11Texture2D*                    g_pDepthStencil;
	ID3D11DepthStencilView*             g_pDepthStencilView;
	IDXGIFactory*						g_dxgiFactory;
	
	ID3D11VertexShader*                 g_pStaticVertexShaderModel = NULL;
	ID3D11PixelShader*                  g_pStaticPixelShaderModel = NULL;
	ID3D11InputLayout*                  g_pStaticVertexLayoutModel = NULL;


	ID3D11VertexShader*                 g_pSkinnedVertexShaderModel = NULL;
	ID3D11PixelShader*                  g_pSkinnedPixelShaderModel = NULL;
	ID3D11InputLayout*                  g_pSkinnedVertexLayoutModel = NULL;

	ID3D11BlendState*					g_pBlendState = NULL;
	
	ID3D11Buffer*                       g_pCBSkinnedModelMatrices = NULL;
	ID3D11ShaderResourceView*           g_pTextureRV = NULL;
	ID3D11SamplerState*                 g_pSamplerLinear = NULL;

	XMFLOAT4X4							g_Projection;

	int animationIndex = 0;
	bool play = true;
	bool rotateCamera = false;
	bool renderLines = false;
	bool writeModel = false;

	Renderer(HINSTANCE hinst, HWND hwnd);
	~Renderer();
	HRESULT InitDevice();
	
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShaders(WCHAR* file, ID3D11VertexShader** vertexShader, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements, ID3D11InputLayout** inputLayout, ID3D11PixelShader** pixelShader);
	void SetViewport(int width, int height);
	void SetProjection(int width, int height);

	ID3D11Buffer** GetViewTransformBuffer();
	ID3D11Buffer** GetProjectionTransformBuffer();
	std::shared_ptr<Camera> GetCamera();
	Entity* Renderer::Pick(int sx, int sy);
	void Renderer::Pick(int sx, int sy, int sw, int sh, std::vector<Entity*>& hitElements);
	bool ProjectPickingRayToPlane(int x, int y, CXMVECTOR planePoint, CXMVECTOR planeNormal, XMVECTOR& intersectionPoint);

	void RegisterModelDirectory(const std::string& directory);
	std::string LoadModel(const std::string& directory);
	void LoadModels(const std::string& directory);
	SkinnedMeshInstance* CreateSkinnedMeshInstance(const std::string& meshName);
	StaticMeshInstance* CreateStaticMeshInstance(const std::string& meshName);

	HRESULT DrawText(const std::wstring& text);
	HRESULT Renderer::DrawRect(int left, int top, int right, int bottom);

	void BeginRenderPass();
	void CompleteRenderPass();

	const std::unordered_map<std::string, SkinnedMesh*>& GetSkinnedMeshLookup();
	const std::unordered_map<std::string, StaticMesh*>& GetStaticMeshLookup();
};



