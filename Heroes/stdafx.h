// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#undef min
#undef max
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DDSTextureLoader.h>
#include <dwrite_1.h>
#include <d2d1_1.h>

// C RunTime Header Files
#include <tchar.h>
#include <sstream>
#include <string>
#include <vector>
#include <istream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <memory>
#include <sys/stat.h>
#include <thread>

// TODO: reference additional headers your program requires here
