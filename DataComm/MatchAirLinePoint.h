///ƥ�亽����
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
	///�������µ�λ����Ϣ����ȡ��ǰ�ĺ�����
	void MatchCurrentAirLinePT(const COORDINATE position);
public:
	UINT DPThdImp(LPVOID pParam);
	 /** ���������߳�  
     *  
     *   
     *  @return: bool  �����Ƿ�ɹ�  
     *  @note:   ���߳��Ѿ����ڿ���״̬ʱ,����flase  
     *  @see:      
     */ 
	bool OpenMatchProcessThread();

	static UINT WINAPI MatchProcessThread( void* pParam );
public:
	///��ǰ�������Ϣ
	CGlobalAirLine *m_pGlobalAirLine;

	CSystemParam m_sysParam;
	
	///���ݽ����¼�
	HANDLE	m_hEvtsMatchThread;

    /** �߳̾�� */   
    HANDLE  m_hMachProcessThread;  

	/** �߳��˳���־���� */   
    static bool s_bExitMatch; 

	CDataContainer<string*> *m_pContainer;
};

