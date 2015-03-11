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
	* @brief: 添加串口数据观察者
    * @description:	添加串口数据观察者
	* @param: pObserver 观察者指针
	* @return: N/A
	*/	
	void AddObserver(IDataUpdate *pObserver);
	
	/**
	* @brief: 删除串口数据观察者
    * @description:	删除串口数据观察者
	* @param: pObserver 观察者指针
	* @return: N/A
	*/	
	void RemoveObserver(IDataUpdate *pObserver);	
	/**
	* @brief: 通知所有串口数据观察者数据更新
    * @description:	通知所有串口数据观察者数据更新
	* @param: pBuffer 数据缓存
	* @param: size 数据长度
	* @return: N/A
	*/	
	void Notify(char *pBuffer, int size);
public:
	UINT DPThdImp(LPVOID pParam);
	 /** 开启监听线程  
     *  
     *  本监听线程完成对串口数据的监听,并将接收到的数据打印到屏幕输出  
     *  @return: bool  操作是否成功  
     *  @note:   当线程已经处于开启状态时,返回flase  
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

    /** 线程句柄 */   
    HANDLE  m_hDataProcessThread;  
	
	CDataProcess m_dataProcess;

	//观察者容器
	vector<IDataUpdate*> m_vecObservers;
public:
	/** 线程退出标志变量 */   
    static bool s_bExit; 

};

