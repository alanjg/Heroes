#pragma once

#include "resource.h"

extern HINSTANCE                           g_hInst;
extern HWND                                g_hWnd;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
void InitScene();

class NetworkClient;
class Renderer;
class Game;
class EntityDefinitionManager;
extern grpc::ClientContext* g_grpcContext;
extern NetworkClient* g_networkClient;
extern bool g_useNetwork;
extern std::shared_ptr<Renderer> g_renderer;
extern std::shared_ptr<Game> g_game;
extern std::shared_ptr<EntityDefinitionManager> g_entityDefinitionManager;
extern std::string g_modelDir;
extern std::string g_dataDir;