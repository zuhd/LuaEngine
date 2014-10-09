/************************************************************************/
/* CopyRight@2009 by Leogon. All right reserved.
/*@Filename: 
/*@Author: Hefe
/*@Date: 12/21/2009
/*@Description:                                                                    
/************************************************************************/
#ifndef LUAMANAGER_H
#define LUAMANAGER_H

struct lua_State;
////////////////////////////////////////Lua 调用的C++函数///////////////////////////////////
API int GetPlayerMoney(lua_State* L);
API int GetPlayerScore(lua_State* L);
API int GetPlayerNickName(lua_State* L);
API int GetPlayerAvatar(lua_State* L);
API int GetPlayerPartyID(lua_State* L);
API int GetPlayerVIPLv(lua_State* L);
API int GetPlayerBindMac(lua_State* L);
API int GetPlayerDegree(lua_State* L);
API int GetPlayerHornor(lua_State* L);
API int TraceMsg(lua_State* L);
API int SendMsg2Player(lua_State* L);
API int IsFrozenGame(lua_State* L);
API int ClearMinus(lua_State* L);
API int AddContainerVol(lua_State* L);
API int UpdateAvatar(lua_State* L);
API int GetPlayerHandQuestCnt(lua_State* L);
API int GetPlayerQuestStatus(lua_State* L);
API int AcceptQuest(lua_State* L);
API int UpdatePlayerQuestStatus(lua_State* L);
API int AddPlayerMoney(lua_State* L);
API int AddPlayerBonus(lua_State* L);
API int AddPlayerHonor(lua_State* L);
API int GetPlayerGender(lua_State* L);
API int GetPlayerFaceID(lua_State* L);
API int GetPlayerImage(lua_State* L);
API int GetPlayerCoffer(lua_State* L);
API int GetPlayerCash(lua_State* L);
API int GetPlayerBonus(lua_State* L);
API int GetPlayerLastLoginIP(lua_State* L);
API int GetPlayerRelationSet(lua_State* L);
API int CanAddContainerGoods(lua_State* L);
API int AddContainerGoods(lua_State* L);
API int GetContainerVol(lua_State* L);
////////////////////////////////////一些常用的数学运算库///////////////////////////
API int GetBitAdd(lua_State* L);
API int GetBitOr(lua_State* L);

class CLuaManager : public ILuaManager
{
public:
	CLuaManager();
	virtual ~CLuaManager();
	virtual bool InitManager(IGameWorld* pGameWorld);
	virtual void Release();	
	virtual bool CallFile(const char *szFileName);	
	virtual bool CallFunction(const char *szFuncName, SScriptParamObj *pIn, 
		int nInNum,	SScriptParamObj *pRet, int nRetNum);	
	virtual bool CallString(const char *szString);	
public:
	void PushTable(SScriptParamObj* pIn, int nInNum);
	void ReturnTable(SScriptParamObj* pRet);
	void FormatError();
	void OutputError(const char* szErrType);	
	lua_State* GetMasterState();
	void RegFunction();
private:	
	lua_State* m_pMasterState;
	leogon::CFactory m_cBaseFac;
	char m_arrLastErr[256];	
};
extern "C" P_EXPORT_DLL ILuaManager* CreateLuaManager();
extern CLuaManager* g_pLuaManager;
#endif