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
	///��ʼ��UDP
	bool Init( int port );

	///��������
	void SendData(const char* Buffer, int size);

	bool OpenListenThread();

	static UINT WINAPI RecieveData(void* pParam);  
	///�˳�UDP
	void Exit();
public:
	//�����׽���
	SOCKET m_sockListener;  

	SOCKET m_ServerSocket;

	//ָ��ͨ�Ŷ���Ľṹ��ָ��
	SOCKADDR_IN m_saUdpServ;

	bool m_bExitFlag;

	/** �߳̾�� */   
    HANDLE    m_hListenThread;  

	static bool s_bListenExit;

	System_Status m_lastSYSStatus;

	char m_strConfig[128];
};

