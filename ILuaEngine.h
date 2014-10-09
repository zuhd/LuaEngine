/************************************************************************/
/* CopyRight@2009 by Leogon. All right reserved.
/*@Filename: 
/*@Author: Hefe
/*@Date: 5/4/2010
/*@Description: Lua && c++ �ӿ��ļ�                                                                    
/************************************************************************/
#ifndef	ILUAENGINE_H
#define	ILUAENGINE_H

#define GTC_LP		0x0000	// ָ��
#define GTC_INT		0x0001	// ����
#define GTC_DOUBLE	0x0002	// ������
#define GTC_STRING	0x0003	// �ַ���

// �ű��������Ͷ���
enum
{
	SD_NUMBER = 0,	// ��������
	SD_STRING,		// �ַ�������
	SD_TABLE,		//��
};

struct SSDTable
{
	int nNum;
	void* pValue;	
};

// �ű���������
struct SScriptParamObj
{
	int nType;			// ��������, SD_NUMBER ���� SD_STRING

	union UScriptParam	// ����ֵ
	{
		int		nNumber;		// ����
		char	szString[64];	// �ַ���
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
	*	@Param: szFileName - �ű��ļ���
	*	@Return: NULL
	*	@Description: ����һ���ű�
	*/
	virtual bool CallFile(const char *szFileName) = 0;

	/*
	*	@Param: szFuncName - ������ pIn, nInNum - ��������б�ָ���Լ�����
	*	@Return: pRet, nRetNum - ���ز����б�ָ���Լ�����
	*	@Description: ����һ������
	*/
	virtual bool CallFunction(const char *szFuncName, SScriptParamObj *pIn, 
		int nInNum,	SScriptParamObj *pRet, int nRetNum) = 0;

	/*
	*	@Param: szString - �ַ���ָ��
	*	@Return: NULL
	*	@Description: ����һ���ַ���
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