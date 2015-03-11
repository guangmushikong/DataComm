#pragma once

#include "stdafx.h"
#include "IDataUpdate.h"
#include "INotify.h"
#include "DataContainer.h"
#include <string>
#include "COMMSTRUCT.h"
#include "DataProcess.h"

using namespace std;

#define MAX_BUFFER_SIZE 1024

class CComReceiver : public IDataUpdate
{
public:
	CComReceiver(void);
	~CComReceiver(void);
public:
	virtual void DataUpdate( char *pBuffer, int size );
	/**
	* @brief: ��Ӵ������ݹ۲���
    * @description:	��Ӵ������ݹ۲���
	* @param: pObserver �۲���ָ��
	* @return: N/A
	*/	
	void AddObserver(IDataUpdate *pObserver);
	
	/**
	* @brief: ɾ���������ݹ۲���
    * @description:	ɾ���������ݹ۲���
	* @param: pObserver �۲���ָ��
	* @return: N/A
	*/	
	void RemoveObserver(IDataUpdate *pObserver);	
	/**
	* @brief: ֪ͨ���д������ݹ۲������ݸ���
    * @description:	֪ͨ���д������ݹ۲������ݸ���
	* @param: pBuffer ���ݻ���
	* @param: size ���ݳ���
	* @return: N/A
	*/	
	void Notify(char *pBuffer, int size);
public:
	UINT DPThdImp(LPVOID pParam);
	 /** ���������߳�  
     *  
     *  �������߳���ɶԴ������ݵļ���,�������յ������ݴ�ӡ����Ļ���  
     *  @return: bool  �����Ƿ�ɹ�  
     *  @note:   ���߳��Ѿ����ڿ���״̬ʱ,����flase  
     *  @see:      
     */ 
	bool OpenDataProcessThread();

	static UINT WINAPI DataProcessThread( void* pParam );
public:
	int FormatTrans(const string &data, COMM_MESSAGE *pMsg);
private:
	char	m_strBuffer[MAX_BUFFER_SIZE];
	
	int		m_iBufferLen;
	
	int		m_iBinLen;

	CDataContainer<string*> *m_pContainer;

	HANDLE	m_hEvtsDPThread[2];

    /** �߳̾�� */   
    HANDLE  m_hDataProcessThread;  
	
	CDataProcess m_dataProcess;

	//�۲�������
	vector<IDataUpdate*> m_vecObservers;
public:
	/** �߳��˳���־���� */   
    static bool s_bExit; 

};

