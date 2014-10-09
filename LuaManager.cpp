#include "stdafx.h"
#include "LuaManager.h"

ILogger*		g_pLogger = 0;
IGameWorld*		g_pGameWorld = 0;
CLuaManager* g_pLuaManager = NULL;

extern "C" P_EXPORT_DLL ILuaManager* CreateLuaManager()
{
	if(g_pLuaManager == NULL)
	{
		g_pLuaManager = new CLuaManager();
	}
	return g_pLuaManager;
}

CLuaManager::CLuaManager ():
m_cBaseFac("liblg_basesystem.so")
{		
	m_pMasterState = NULL;
}

bool CLuaManager::InitManager(IGameWorld* pGameWorld)
{
	//装载日志模块
	g_pLogger = m_cBaseFac.createmodule<ILogger>("CreateLogSystem");
	if(g_pLogger == 0 || !g_pLogger->Init(0, "LuaEngineLog", 0))
	{
		glog.log("Init Logger Failed!");
		goto FAILED;
	}
	if (pGameWorld == NULL)
	{
		glog.log("Load Gameworld Error");
		goto FAILED;
	}
	g_pGameWorld = pGameWorld;
	m_pMasterState = lua_open();
	if (m_pMasterState)
	{
		luaopen_base(m_pMasterState);
		luaL_openlibs(m_pMasterState);		
	}
	RegFunction();	
	return true;
FAILED:
	SAFE_RELEASE(g_pLogger);
	return false;
}

void CLuaManager::RegFunction()
{
	lua_register(m_pMasterState, "API_GetPlayerMoney", GetPlayerMoney);
	lua_register(m_pMasterState, "API_GetPlayerScore", GetPlayerScore);	
	lua_register(m_pMasterState, "API_GetPlayerNickName", GetPlayerNickName);
	lua_register(m_pMasterState, "API_GetPlayerAvatar", GetPlayerAvatar);
	lua_register(m_pMasterState, "API_GetPlayerPartyID", GetPlayerPartyID);
	lua_register(m_pMasterState, "API_GetPlayerVIPLv", GetPlayerVIPLv);
	lua_register(m_pMasterState, "API_GetPlayerBindMac", GetPlayerBindMac);
	lua_register(m_pMasterState, "API_GetPlayerDegree", GetPlayerDegree);
	lua_register(m_pMasterState, "API_GetPlayerHornor",GetPlayerHornor);
	lua_register(m_pMasterState, "API_TraceMsg", TraceMsg);
	lua_register(m_pMasterState, "API_SendMsg2Player", SendMsg2Player);
	lua_register(m_pMasterState, "API_IsFrozenGame", IsFrozenGame);
	lua_register(m_pMasterState, "API_ClearMinus", ClearMinus);
	lua_register(m_pMasterState, "API_AddContainerVol", AddContainerVol);
	lua_register(m_pMasterState, "API_UpdateAvatar", UpdateAvatar);
	lua_register(m_pMasterState, "API_GetPlayerHandQuestCnt", GetPlayerHandQuestCnt);
	lua_register(m_pMasterState, "API_GetPlayerQuestStatus", GetPlayerQuestStatus);
	lua_register(m_pMasterState, "API_AcceptQuest", AcceptQuest);
	lua_register(m_pMasterState, "API_UpdatePlayerQuestStatus", UpdatePlayerQuestStatus);
	lua_register(m_pMasterState, "API_AddPlayerMoney", AddPlayerMoney);
	lua_register(m_pMasterState, "API_AddPlayerBonus", AddPlayerBonus);
	lua_register(m_pMasterState, "API_AddPlayerHonor", AddPlayerHonor);
	lua_register(m_pMasterState, "API_GetPlayerGender", GetPlayerGender);
	lua_register(m_pMasterState, "API_GetPlayerFaceID", GetPlayerFaceID);
	lua_register(m_pMasterState, "API_GetPlayerImage", GetPlayerImage);
	lua_register(m_pMasterState, "API_GetPlayerCoffer", GetPlayerCoffer);
	lua_register(m_pMasterState, "API_GetPlayerCash", GetPlayerCash);
	lua_register(m_pMasterState, "API_GetPlayerBonus", GetPlayerBonus);
	lua_register(m_pMasterState, "API_GetPlayerLastLoginIP", GetPlayerLastLoginIP);
	lua_register(m_pMasterState, "API_GetPlayerRelationSet", GetPlayerRelationSet);
	lua_register(m_pMasterState, "API_CanAddContainerGoods", CanAddContainerGoods);
	lua_register(m_pMasterState, "API_AddContainerGoods", AddContainerGoods);
	lua_register(m_pMasterState, "API_GetContainerVol", GetContainerVol);
	lua_register(m_pMasterState, "API_GetBitAdd", GetBitAdd);
	lua_register(m_pMasterState, "API_GetBitOr", GetBitOr);
}

CLuaManager::~CLuaManager()
{
	
}

void CLuaManager::Release()
{	
	lua_close(m_pMasterState);
	delete this;
}

bool CLuaManager::CallFile(const char *szFileName)
{
	assert(m_pMasterState);	
	if (szFileName == NULL)
	{
		return false;
	}
	int nStatus = luaL_dofile(m_pMasterState, szFileName);
	if (nStatus == 0)
	{		
	}
	else
	{
		FormatError();
		OutputError("Syntax Error");
		return false;
	}
	return true;
}

bool CLuaManager::CallString(const char *szString)
{	
	assert(m_pMasterState);
	if (szString == NULL)
	{
		return false;
	}
	int nStatus = luaL_loadbuffer(m_pMasterState, szString, strlen(szString), "Console");
	if (nStatus == 0) 
	{
		nStatus = lua_pcall(m_pMasterState, lua_gettop(m_pMasterState) - 1, 0, 0);
	}
	else
	{
		FormatError();
		return false;
	}

	return true;
}

bool CLuaManager::CallFunction(const char *szFuncName, SScriptParamObj *pIn, 
							  int nInNum, SScriptParamObj *pRet, int nRetNum)
{
	if (szFuncName == NULL)
	{
		return false;
	}	
	assert(m_pMasterState);
	lua_getglobal(m_pMasterState, szFuncName);
	for (int i = 0; i < nInNum; i++)
	{
		// 参数的三种类型
		switch (pIn[i].nType)
		{
		case SD_NUMBER:
			lua_pushnumber(m_pMasterState, pIn[i].unValue.nNumber);
			break;
		case SD_STRING:
			lua_pushstring(m_pMasterState, pIn[i].unValue.szString);
			break;
		case SD_TABLE:	
			// 现在栈顶创建一个新的表
			lua_newtable(m_pMasterState);
			int nSize = pIn[i].unValue.stTable.nNum;	
			SScriptParamObj* pData = (SScriptParamObj*)pIn[i].unValue.stTable.pValue;
			PushTable(pData, nSize);
			break;
		}
	}
	int nStatus = lua_pcall(m_pMasterState, nInNum, nRetNum, 0);

	for (int i = nRetNum - 1; i >= 0; i--)
	{
		// 参数的三种类型,pop的顺序,完全靠直觉
		switch (pRet[i].nType)
		{
		case SD_NUMBER:
			pRet[i].unValue.nNumber = lua_tonumber(m_pMasterState, -1);
			lua_pop(m_pMasterState, 1);
			break;
		case SD_STRING:
			strcpy(pRet[i].unValue.szString, lua_tostring(m_pMasterState, -1));
			lua_pop(m_pMasterState, 1);
			break;		
		case SD_TABLE:
			ReturnTable(&pRet[i]);
			lua_pop(m_pMasterState, 1);
			break;
		}
	}

	if (nStatus != 0)
	{
		FormatError();
		OutputError("Runtime Error:");
		return false;
	}
	return true;
}
// 严重注意 在传参数的时候 如果是传表
// 那么表里的嵌套数据也必须是SScriptParamObj类型
void CLuaManager::PushTable(SScriptParamObj *pIn, int nInNum)
{
	for (int i = 0; i < nInNum; i++)
	{
		// 参数的三种类型
		switch (pIn[i].nType)
		{
		case SD_NUMBER:
			// 添加key和value，下标从1开始
			lua_pushnumber(m_pMasterState, i + 1);
			lua_pushnumber(m_pMasterState, pIn[i].unValue.nNumber);
			lua_rawset(m_pMasterState, -3);
			break;
		case SD_STRING:
			lua_pushnumber(m_pMasterState, i + 1);
			lua_pushstring(m_pMasterState, pIn[i].unValue.szString);
			lua_rawset(m_pMasterState, -3);
			break;
		case SD_TABLE:	
			lua_pushnumber(m_pMasterState, i + 1);
			lua_newtable(m_pMasterState);
			int nSize = pIn[i].unValue.stTable.nNum;	
			SScriptParamObj* pData = (SScriptParamObj*)pIn[i].unValue.stTable.pValue;
			PushTable(pData, nSize);
			lua_rawset(m_pMasterState, -3);
			break;
		}
	}
}

// 不再采用LuaEngine分配内存的规则
// 因为这样外部程序去删除Lua的内存
// 会出错 如果是表的话 外部只要分配
// 足够大的内存就可以了 这点开发者注意
// 如果是跌代表的话 每一次层的内存都要
// 分配好 和参数表的规则是一致的
void CLuaManager::ReturnTable(SScriptParamObj* pRet)
{
	// 获取到表的索引
	int nNum = 0;			
	int nIndex = lua_gettop(m_pMasterState);
	lua_pushnil(m_pMasterState);
	// 先得到数组的长度
	while (lua_next(m_pMasterState, nIndex) != 0)    
	{    				
		nNum++;
		//移除 'value' ；保留 'key' 做下一次迭代
		lua_pop(m_pMasterState, 1); 
	} 
	nIndex = lua_gettop(m_pMasterState);
	// 这时候栈顶还是表
	lua_pushnil(m_pMasterState);
	//SScriptParamObj* pObject = new SScriptParamObj[nNum];
	SScriptParamObj* pObject = (SScriptParamObj*)pRet->unValue.stTable.pValue;
	//pRet->unValue.stTable.pValue = pObject;
	//取lua栈中真正的参数值 要保证参数分配的空间足够大
	pRet->unValue.stTable.nNum = nNum;
	nNum = 0;
	while (lua_next(m_pMasterState, nIndex) != 0)    
	{    
		// 'key' (索引-2) 和 'value' (索引-1)	
		// 只对Value感兴趣
		if (lua_type(m_pMasterState, -1) == LUA_TSTRING)    
		{ 					
			pObject[nNum++] = lua_tostring(m_pMasterState, -1);    
		}    
		else if (lua_type(m_pMasterState, -1) == LUA_TNUMBER)    
		{   					 
			pObject[nNum++] = (int)lua_tonumber(m_pMasterState, -1);   
		}    
		else if (lua_type(m_pMasterState, -1) == LUA_TTABLE)
		{    
			ReturnTable(&pObject[nNum++]);
		}  
		else
		{
			nNum++;
		}
		//移除 'value' ；保留 'key' 做下一次迭代 
		lua_pop(m_pMasterState, 1); 
	}  	
}

void CLuaManager::FormatError()
{
	const char*	 szErr = lua_tostring(m_pMasterState, -1);
	if (szErr == NULL) 
	{
		szErr = "(error with no message)";
	}
	lua_pop(m_pMasterState, 1);
	strcpy(m_arrLastErr, szErr);
}

void CLuaManager::OutputError(const char* szErrType)
{
	glog.log("%s %s \n", szErrType, m_arrLastErr);
}


////////////////////////////////////////Lua 调用的C++函数///////////////////////////////////
int TraceMsg(lua_State* L)
{
	assert (lua_isstring(L, 1));
	const char* szMsg = lua_tostring(L, 1);

	// get caller
	lua_Debug ar;
	memset(&ar, 0, sizeof(ar));
	lua_getstack(L, 1, &ar);
	lua_getinfo(L, "Snl", &ar);

	// debug output
	const char* str = ar.source;
	g_pLogger->Log("Trace['%s'] at File[%s] Line[%d]\n", szMsg, str, ar.currentline);
	return 0;
}

int GetPlayerMoney(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int64 nMoney = g_pGameWorld->GetPlayerMoney(nPID);
	lua_pushnumber(L, nMoney);
	return 1;
}

int GetPlayerScore(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nGameID = lua_tonumber(L, 2);
	int nScore = g_pGameWorld->GetPlayerScore(nPID, nGameID);
	lua_pushnumber(L, nScore);
	return 1;
}

int GetPlayerNickName(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	string strName = g_pGameWorld->GetPlayerNickName(nPID);
	lua_pushstring(L, strName.c_str());
	return 1;
}

int GetPlayerAvatar(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	string strAvatar = g_pGameWorld->GetPlayerAvatar(nPID);
	lua_pushstring(L, strAvatar.c_str());
	return 1;
}

int GetPlayerPartyID(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nID = g_pGameWorld->GetPlayerPartyID(nPID);
	lua_pushnumber(L, nID);
	return 1;
}

int GetPlayerVIPLv(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nVIPLv = g_pGameWorld->GetPlayerVIPLv(nPID);
	lua_pushnumber(L, nVIPLv);
	return 1;
}

int GetPlayerBindMac(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	string strMac = g_pGameWorld->GetPlayerBindMac(nPID);
	lua_pushstring(L, strMac.c_str());
	return 1;
}

int GetPlayerDegree(lua_State* L)
{
	// 获得参数个数
	int nParamNum = lua_gettop(L);
	// 获取第一个参数
	int nPID = lua_tonumber(L, 1);	

	int nDegree = g_pGameWorld->GetPlayerDegree(nPID);
	lua_pushnumber(L, nDegree);
	// 返回参数个数
	return 1;
}

int GetPlayerHornor(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nHornor = g_pGameWorld->GetPlayerHornor(nPID);
	lua_pushnumber(L, nHornor);
	return 1;
}

int SendMsg2Player(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	const char* szMsg = lua_tostring(L, 2);	
	int nLevel = lua_tonumber(L, 3);
	int nType = lua_tonumber(L, 4);
	g_pGameWorld->SendMsg2Player(nPID, szMsg, nLevel, nType);
	return 0;
}

int IsFrozenGame(lua_State* L)
{
	int nServerID = lua_tonumber(L, 1);
	int nRet = g_pGameWorld->IsFrozenGame(nServerID);
	lua_pushnumber(L, nRet);
	return 1;
}

int ClearMinus(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nServerID = lua_tonumber(L, 2);
	g_pGameWorld->ClearMinus(nPID, nServerID);
	return 0;
}

int AddContainerVol(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nCID = lua_tonumber(L, 2);
	int nAdd = lua_tonumber(L, 3);
	int nRet = g_pGameWorld->AddContainerVol(nPID, nCID, nAdd);
	lua_pushnumber(L, nRet);
	return 1;
}

int UpdateAvatar(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	string strAvatar = lua_tostring(L, 2);
	g_pLogger->Log("In LuaEngine [%d] update avatar[%s]", nPID, strAvatar.c_str());
	g_pGameWorld->UpdateAvatar(nPID, strAvatar);
	return 0;
}

int GetPlayerHandQuestCnt(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nCount = g_pGameWorld->GetPlayerHandQuestCnt(nPID);
	lua_pushnumber(L, nCount);
	return 1;
}

int GetPlayerQuestStatus(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nQuestID = lua_tonumber(L, 2);
	int nStatus = g_pGameWorld->GetPlayerQuestStatus(nPID, nQuestID);
	lua_pushnumber(L, nStatus);
	return 1;
}

int AcceptQuest(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nQuestID = lua_tonumber(L, 2);
	g_pGameWorld->AcceptQuest(nPID, nQuestID);
	return 0;
}

int UpdatePlayerQuestStatus(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nQuestID = lua_tonumber(L, 2);
	int nStatus = lua_tonumber(L, 3);
	g_pGameWorld->UpdatePlayerQuestStatus(nPID, nQuestID, nStatus);
	return 0;
}

int AddPlayerMoney(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nMoney = lua_tonumber(L, 2);
	int cFlag = lua_tonumber(L, 3);
	g_pGameWorld->AddPlayerMoney(nPID, nMoney, (char)cFlag);
	return 0;
}

int AddPlayerBonus(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nBonus = lua_tonumber(L, 2);
	int cFlag = lua_tonumber(L, 3);
	g_pGameWorld->AddPlayerBonus(nPID, nBonus, (char)cFlag);
	return 0;
}

int AddPlayerHonor(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nHonor = lua_tonumber(L, 2);
	int cFlag = lua_tonumber(L, 3);
	g_pGameWorld->AddPlayerHornor(nPID, nHonor, (char)cFlag);
	return 0;
}

int GetPlayerGender(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	char cGender = g_pGameWorld->GetPlayerGender(nPID);
	lua_pushnumber(L, cGender);
	return 1;
}

int GetPlayerFaceID(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nFaceID = g_pGameWorld->GetPlayerFaceID(nPID);
	lua_pushnumber(L, nFaceID);
	return 1;
}

int GetPlayerImage(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	string strImage = g_pGameWorld->GetPlayerImage(nPID);
	lua_pushstring(L, strImage.c_str());
	return 1;
}

int GetPlayerCoffer(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int64 nCoffer = g_pGameWorld->GetPlayerCoffer(nPID);
	lua_pushnumber(L, nCoffer);
	return 1;
}

int GetPlayerCash(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nCash = g_pGameWorld->GetPlayerCash(nPID);
	lua_pushnumber(L, nCash);
	return 1;
}

int GetPlayerBonus(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nBonus = g_pGameWorld->GetPlayerBonus(nPID);
	lua_pushnumber(L, nBonus);
	return 1;
}

int GetPlayerLastLoginIP(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nIP = g_pGameWorld->GetPlayerLastLoginIP(nPID);
	lua_pushnumber(L, nIP);
	return 1;
}

int GetPlayerRelationSet(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	char cSet = g_pGameWorld->GetPlayerRelationSet(nPID);
	lua_pushnumber(L, cSet);
	return 1;
}

int CanAddContainerGoods(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nGoodsID = lua_tonumber(L, 2);
	int nCount = lua_tonumber(L, 3);
	bool bSuccess = g_pGameWorld->CanAddContainerGoods(nPID, nGoodsID, nCount);
	lua_pushboolean(L, bSuccess);
	return 1;
}

int AddContainerGoods(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nGoodsID = lua_tonumber(L, 2);
	int nCount = lua_tonumber(L, 3);
	char cBind = lua_tonumber(L, 4);
	bool bSuccess = g_pGameWorld->AddContainerGoods(nPID, nGoodsID, nCount, cBind);
	lua_pushboolean(L, bSuccess);
	return 1;
}

int GetContainerVol(lua_State* L)
{
	int nPID = lua_tonumber(L, 1);
	int nCID = lua_tonumber(L, 2);
	int nSize = g_pGameWorld->GetContainerVol(nPID, nCID);
	lua_pushnumber(L, nSize);
	return 1;
}

int GetBitAdd(lua_State* L)
{
	int nA = lua_tonumber(L, 1);
	int nB = lua_tonumber(L, 2);
	lua_pushnumber(L, nA & nB);
	return 1;
}

int GetBitOr(lua_State* L)
{
	int nA = lua_tonumber(L, 1);
	int nB = lua_tonumber(L, 2);
	lua_pushnumber(L, nA | nB);
	return 1;
}