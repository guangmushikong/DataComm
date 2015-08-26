#pragma once

#include "stdafx.h"
#include "SystemParam.h"
#include "IDataUpdate.h"

using namespace std;
#pragma comment (lib,"ws2_32.lib")

class CUDPServer 
{
public:
	CUDPServer(void);
	~CUDPServer(void);

public:
	///初始化UDP
	bool Init( int port );

	///发送数据
	void SendData(const char* Buffer, int size);

	bool OpenListenThread();

	static UINT WINAPI RecieveData(void* pParam);  
	///退出UDP
	void Exit();
public:
	//创建套接字
	SOCKET m_sockListener;  

	SOCKET m_ServerSocket;

	//指向通信对象的结构体指针
	SOCKADDR_IN m_saUdpServ;

	bool m_bExitFlag;

	/** 线程句柄 */   
    HANDLE    m_hListenThread;  

	static bool s_bListenExit;

	System_Status m_lastSYSStatus;

	char m_strConfig[128];
};

