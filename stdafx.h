#ifndef STDAFX_H
#define STDAFX_H
#include "BaseLibs/Factory.h"
#include "BaseLibs/GlobalDefine.h"
#include "BaseLibs/Serialize.h"
#include "BaseLibs/PacketDefine.h"
#include "BaseLibs/Log.h"
#include "BaseLibs/tinystr.h"
#include "BaseLibs/tinyxml.h"
#include "BaseLibs/ObjectPool.h"
#include "BaseLibs/Utilty.h"
#include "ILogSystem.h"
#include "IGameWorld.h"
#include <time.h>
#include <assert.h>
#include <string>
using namespace std;
#include "./ILuaEngine.h"

extern "C" 
{
#include "./lua/lua.h"
#include "./lua/lualib.h"
#include "./lua/lauxlib.h"
}

#define API extern
extern ILogger*			g_pLogger;
extern IGameWorld*		g_pGameWorld;
#ifdef WIN32
#pragma comment(lib, "../../SrcLibs/BaseModule/lua51.lib")
#ifdef _DEBUG
#pragma comment(lib, "../../SrcLibs/BaseModule/BaseLibs_D.lib")
#else
#pragma comment(lib, "../../SrcLibs/BaseModule/BaseLibs.lib")
#endif
#else
#include <stdlib.h>
#endif
#endif 
