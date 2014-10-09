/************************************************************************/
/* CopyRight@2009 by Leogon. All right reserved.
/*@Filename: 
/*@Author: Hefe
/*@Date: 5/4/2010
/*@Description: Lua && c++ 接口文件                                                                    
/************************************************************************/
#ifndef	ILUAENGINE_H
#define	ILUAENGINE_H

#define GTC_LP		0x0000	// 指针
#define GTC_INT		0x0001	// 整型
#define GTC_DOUBLE	0x0002	// 浮点型
#define GTC_STRING	0x0003	// 字符串

// 脚本数据类型定义
enum
{
	SD_NUMBER = 0,	// 数字类型
	SD_STRING,		// 字符串类型
	SD_TABLE,		//表
};

struct SSDTable
{
	int nNum;
	void* pValue;	
};

// 脚本参数对象
struct SScriptParamObj
{
	int nType;			// 参数类型, SD_NUMBER 或者 SD_STRING

	union UScriptParam	// 参数值
	{
		int		nNumber;		// 数字
		char	szString[64];	// 字符串
		SSDTable stTable;

	} unValue;

	SScriptParamObj()
	{
		memset(this, 0, sizeof(*this));
	}

	~SScriptParamObj()
	{

	}

	void operator = (int nValue)
	{
		nType = SD_NUMBER;
		unValue.nNumber = nValue;
	}

	void operator = (const char *str)
	{
		nType = SD_STRING;
		unValue.szString[0] = 0;

		if (str != NULL)
		{
			strncpy(unValue.szString, str, sizeof(unValue.szString));
		}
	}

	void operator = ( SSDTable& pT )
	{
		nType = SD_TABLE;
		unValue.stTable.nNum = pT.nNum;
		unValue.stTable.pValue = (void *)pT.pValue;	
	}

};

struct IGameWorld;
struct ILuaManager
{
	/*
	 *	@Param: NULL
	 *	@Return: NULL
	 *	@Description: NULL
	 */
	virtual bool InitManager(IGameWorld* pGameWorld) = 0;	
	/*
	*	@Param: szFileName - 脚本文件名
	*	@Return: NULL
	*	@Description: 调用一个脚本
	*/
	virtual bool CallFile(const char *szFileName) = 0;

	/*
	*	@Param: szFuncName - 函数名 pIn, nInNum - 输入参数列表指针以及个数
	*	@Return: pRet, nRetNum - 返回参数列表指针以及个数
	*	@Description: 调用一个函数
	*/
	virtual bool CallFunction(const char *szFuncName, SScriptParamObj *pIn, 
		int nInNum,	SScriptParamObj *pRet, int nRetNum) = 0;

	/*
	*	@Param: szString - 字符串指针
	*	@Return: NULL
	*	@Description: 调用一个字符串
	*/
	virtual bool CallString(const char *szString) = 0;
	/*
	 *	@Param: NULL
	 *	@Return: NULL
	 *	@Description: NULL
	 */
	virtual void Release() = 0;
};
#endif