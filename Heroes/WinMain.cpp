#include "stdafx.h"
#include "WinMain.h"
#include "Renderer.h"
#include "Entity.h"
#include "Camera.h"
#include "Game.h"
#include "InputManager.h"
#include "File.h"
#include "SelectionManager.h"
#include "SkinnedMeshInstance.h"
#include "StaticMeshInstance.h"
#include "ResourceManager.h"

HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
std::shared_ptr<Renderer> g_renderer = nullptr;
std::shared_ptr<Game> g_game = nullptr;
std::shared_ptr<InputManager> g_inputManager = nullptr;
std::shared_ptr<SelectionManager> g_selectionMananger = nullptr;
std::shared_ptr<ResourceManager> g_resourceManager = nullptr;
#define MAX_LOADSTRING 100

TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HEROES, szWindowClass, MAX_LOADSTRING);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	g_game.reset(new Game());
	g_renderer.reset(new Renderer(g_hInst, g_hWnd));
	g_selectionMananger.reset(new SelectionManager(g_renderer.get()));
	g_inputManager.reset(new InputManager(g_game.get(), g_renderer.get(), g_selectionMananger.get()));
	g_resourceManager.reset(new ResourceManager(g_renderer));
	g_resourceManager->LoadResources();
	g_game->SetContext(g_renderer.get(), g_selectionMananger.get(), g_inputManager.get(), g_resourceManager.get());

	if (FAILED(g_renderer->InitDevice()))
	{
		return 0;
	}

	std::string directoryRoot = "c:\\HeroesAnimations\\";
	std::string selection = directoryRoot + "ConvertedEffects\\ArmySelection";
	g_renderer->LoadModel(selection);

	std::string modelDirectory = directoryRoot + "Converted\\";
	g_renderer->RegisterModelDirectory(modelDirectory);
	g_renderer->LoadModels(directoryRoot + "ConvertedMap2\\");

	//InitScene();
	g_game->CreateTestGame();
	static float t = 0.0f;
	
	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			static DWORD dwTimeStart = 0;
			static int frames = 0;
			static float elapsed = 0.0f;
			DWORD dwTimeCur = GetTickCount();
			if (dwTimeStart == 0)
				dwTimeStart = dwTimeCur;
			float oldTime = t;
			t = (dwTimeCur - dwTimeStart) / 1000.0f;
			
			static int fps = 0;
			static float tps = 0;
			static uint64_t triangles = 0;
			frames++;
			elapsed += t - oldTime;
			if (elapsed >= 1.0f)
			{
				fps = static_cast<int>(static_cast<float>(frames) / elapsed);
				tps = triangles / elapsed;
				tps /= 1e6;
				elapsed = 0;
				frames = 0;
				triangles = 0;
			}			
			
			g_inputManager->Update(t - oldTime);
			g_game->Update(t - oldTime);
			g_selectionMananger->Update(t - oldTime);

			g_renderer->BeginRenderPass();
			g_game->Render();
			g_selectionMananger->Render();
			triangles += g_renderer->m_rendererdTriangleCount;
			std::wstringstream text;
			text.precision(3);
			text << L"FPS: " << fps << std::endl << L"TPS: " << tps;
			
			g_renderer->DrawTextW(text.str());
			g_inputManager->Render();
			g_renderer->CompleteRenderPass();
		}
	}

	return (int) msg.wParam;
}


HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR) IDI_HEROES);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"HeroesWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR) IDI_SMALL);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 1920, 1080 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"HeroesWindowClass", L"Heroes", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int width, height, x, y;
	int fwKeys;
	int zDelta;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		g_inputManager->HandleKey(wParam);
		break;

	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		if (g_renderer != 0)
		{
			g_renderer->SetViewport(width, height);
		}
		break;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		g_inputManager->HandleMouseLeftButtonDown(x, y);
		break;

	case WM_LBUTTONUP:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		g_inputManager->HandleMouseLeftButtonUp(x, y);
		break;

	case WM_RBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		g_inputManager->HandleMouseRightButtonDown(x, y);

	case WM_RBUTTONUP:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		g_inputManager->HandleMouseRightButtonUp(x, y);
		break;

	case WM_MOUSEWHEEL:
		fwKeys = GET_KEYSTATE_WPARAM(wParam);
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		g_inputManager->HandleMouseWheel(zDelta);
		break;

	case WM_MOUSEMOVE:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		g_inputManager->HandleMouseMove(x, y);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
/*
void LoadAndInstantiate(const std::string& directory, float x, float y)
{
	std::string modelName = g_renderer->LoadModel(directory);

	StaticMeshInstance* mesh = g_renderer->CreateStaticMeshInstance(modelName);
	Entity* e = new Entity();
	std::shared_ptr<Entity> entity(e);
	mesh->BindEntity(e);
	entity->Initialize(x, y, mesh);
	g_game->entities.push_back(entity);
}
*/

/*
void InitScene()
{
	std::string directoryRoot = "c:\\HeroesAnimations\\";
	std::string selection = directoryRoot + "ConvertedEffects\\ArmySelection";
	g_renderer->LoadModel(selection);

	//LoadAndInstantiate(directoryRoot + "ConvertedMap3\\Gold_Mine", -5, 0);
	std::string modelDirectory = directoryRoot + "Converted\\";
	g_renderer->RegisterModelDirectory(modelDirectory);

	bool renderAllModels = true;
	int limit = 10000;

	if (renderAllModels)
	{
		//g_renderer->LoadModel(directoryRoot + "ConvertedArenas\\Town\\NewHaven\\Monastery_u0r0");
		//g_renderer->LoadModel(directoryRoot + "ConvertedArenas\\Town\\NewHaven\\Monastery_u1r0");
		//g_renderer->LoadModel(directoryRoot + "ConvertedArenas\\Town\\NewHaven\\Monastery_u2r0");
		//g_renderer->LoadModels(directoryRoot + "Converted\\");
		g_renderer->LoadModels(directoryRoot + "ConvertedMap2\\");
		//g_renderer->LoadModels(directoryRoot + "ConvertedMap3\\");
		//g_renderer->LoadModels(directoryRoot + "ConvertedArenas\\Town\\NewHaven");
	
		int c = 0;
		for each(auto p in g_renderer->GetStaticMeshLookup())
		{
			if (p.first == "ArmySelection") continue;
			StaticMeshInstance* mesh = g_renderer->CreateStaticMeshInstance(p.first);
			Entity* e = new Entity();
			std::shared_ptr<Entity> entity(e);
			mesh->BindEntity(e);
			entity->Initialize((c / 12) * 12, (c % 12) * 12, mesh);
			c++;
			g_game->entities.push_back(entity);
			if (c == limit) break;
		}

		for each(auto p in g_renderer->GetSkinnedMeshLookup())
		{
			SkinnedMeshInstance* mesh = g_renderer->CreateSkinnedMeshInstance(p.first);
			Entity* unit = new Entity();
			mesh->BindEntity(unit);
			std::shared_ptr<Entity> entity(unit);
			unit->Initialize((c / 12) * 12, (c % 12) * 12, mesh);
			c++;
			g_game->entities.push_back(entity);
			if (c == limit) break;
		}
	}

	
	for (int i = 0; i < 3; i++)
	{
		SkinnedMeshInstance* angelMesh = g_renderer->CreateSkinnedMeshInstance("Angel");
		Entity* unit = new Entity();
		angelMesh->BindEntity(unit);
		std::shared_ptr<Entity> entity(unit);
		unit->Initialize((i / 12) * 4, (i % 12) * 4, angelMesh);
		g_game->entities.push_back(entity);
	}
	
	SkinnedMeshInstance* footmanMesh = g_renderer->CreateSkinnedMeshInstance("Footman");
	Entity* unit2 = new Entity();
	footmanMesh->BindEntity(unit2);
	std::shared_ptr<Entity> entity2(unit2);
	unit2->Initialize(15, 0, footmanMesh);
	g_game->entities.push_back(entity2);
	
	SkinnedMeshInstance* dragonMesh = g_renderer->CreateSkinnedMeshInstance("BlackDragon");
	Entity* unit3 = new Entity();
	dragonMesh->BindEntity(unit3);
	std::shared_ptr<Entity> entity3(unit3);
	unit3->Initialize(15, 0, dragonMesh);
	g_game->entities.push_back(entity3);	
}*/