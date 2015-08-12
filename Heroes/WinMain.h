#pragma once

#include "resource.h"

extern HINSTANCE                           g_hInst;
extern HWND                                g_hWnd;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
void InitScene();