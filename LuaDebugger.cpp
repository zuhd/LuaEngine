#include "stdafx.h"
#include "LuaDebugger.h"

static void LuaHookCall (lua_State *lua)
{
	lua_Debug ar;

	// Look at call stack
	for (int iLevel = 0; lua_getstack (lua, iLevel, &ar) != 0; ++iLevel)
	{
		if (lua_getinfo (lua, "Snlu", &ar) != 0)
		{
			printf ("%d %s %s %d @%d %s\n", iLevel, ar.namewhat, ar.name, ar.nups, ar.linedefined, ar.short_src);
		}
	}
}

static void LuaHookRet (lua_State *lua)
{

}

static void LuaHookLine (lua_State *lua)
{
	lua_Debug ar;
	lua_getstack (lua, 0, &ar);

	if (lua_getinfo (lua, "Sl", &ar) != 0)
	{
		printf ("exe %s on line %d\n", ar.short_src, ar.currentline);
	}
}

static void LuaHookCount (lua_State *lua)
{
	LuaHookLine (lua);
}

static void LuaHook (lua_State *lua, lua_Debug *ar)
{
	// Handover to the correct hook
	switch (ar->event)
	{
	case LUA_HOOKCALL:
		LuaHookCall (lua);
		break;
	case LUA_HOOKRET:
	case LUA_HOOKTAILRET:
		LuaHookRet (lua);
		break;
	case LUA_HOOKLINE:
		LuaHookLine (lua);
		break;
	case LUA_HOOKCOUNT:
		LuaHookCount (lua);
		break;
	}
}

CLuaDebugger::CLuaDebugger(lua_State* pState) : 
m_nCountMask(10), 
m_pState(pState)
{
	// Clear all current hooks
	if (m_pState != NULL)
	{		
		lua_sethook (m_pState, LuaHook, 0, m_nCountMask);
	}
}

CLuaDebugger::~CLuaDebugger (void)
{
	// Clear all current hooks
	if (m_pState != NULL)
	{
		lua_sethook (m_pState,LuaHook, 0, m_nCountMask);
	}
}

void CLuaDebugger::SetHooksFlag (int nMask)
{
	// Set hooks
	lua_sethook (m_pState, LuaHook, nMask, m_nCountMask);
}

void CLuaDebugger::OutputError(int nErrCode)
{
	switch (nErrCode)
	{
	case LUA_ERRRUN:
		glog.log("LUA_ERRRUN\n");
		break;
	case LUA_ERRMEM:
		glog.log("LUA_ERRMEM\n");
		break;
	case LUA_ERRERR:
		glog.log("LUA_ERRERR\n");
		break;
	}

	// Get the error string that appears on top of stack when a function
	// fails to run
	glog.log("Error: %s\n", lua_tostring (m_pState, -1));
}