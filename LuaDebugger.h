/************************************************************************/
/* CopyRight@2009 by Leogon. All right reserved.
/*@Filename: 
/*@Author: Hefe
/*@Date: 5/4/2010
/*@Description: LUAµ÷ÊÔ¹¦ÄÜ                                                                   
/************************************************************************/
#ifndef LUADEBUGGER_H
#define LUADEBUGGER_H
enum
{
	DBG_MASK_CALL = LUA_MASKCALL,
	DBG_MASK_RET = LUA_MASKRET,
	DBG_MASK_LINE = LUA_MASKLINE,
	DBG_MASK_COUNT = LUA_MASKCOUNT
};
struct lua_State;
class CLuaDebugger
{
public:
	CLuaDebugger(lua_State* pState);
	virtual ~CLuaDebugger(void);

	void SetHooksFlag(int nMask);
	void SetCount(int nCount);
	void OutputError(int nErrCode);
protected:
	int m_nCountMask;
	lua_State* m_pState;
};
#endif