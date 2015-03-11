///匹配航迹点
#pragma once

#include "stdafx.h"
#include "IDataUpdate.h"
#include "COMMSTRUCT.h"
#include "DataContainer.h"
#include "GlobalAirLine.h"
#include "SystemParam.h"


//class CSystemParam;

class CMatchAirLinePoint : public IDataUpdate
{
public:
	CMatchAirLinePoint(void);
	~CMatchAirLinePoint(void);

public:
	virtual void DataUpdate( char *pBuffer, int size );
public:
	///根据最新的位置信息，获取当前的航迹点
	void MatchCurrentAirLinePT(const COORDINATE position);
public:
	UINT DPThdImp(LPVOID pParam);
	 /** 开启监听线程  
     *  
     *   
     *  @return: bool  操作是否成功  
     *  @note:   当线程已经处于开启状态时,返回flase  
     *  @see:      
     */ 
	bool OpenMatchProcessThread();

	static UINT WINAPI MatchProcessThread( void* pParam );
public:
	///当前拍摄点信息
	CGlobalAirLine *m_pGlobalAirLine;

	CSystemParam m_sysParam;
	
	///数据接收事件
	HANDLE	m_hEvtsMatchThread;

    /** 线程句柄 */   
    HANDLE  m_hMachProcessThread;  

	/** 线程退出标志变量 */   
    static bool s_bExitMatch; 

	CDataContainer<string*> *m_pContainer;
};

