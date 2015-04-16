#pragma once

#include "stdafx.h"

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

	///�˳�UDP
	void Exit();
private:
	//�����׽���
	SOCKET m_sockListener;  

	//ָ��ͨ�Ŷ���Ľṹ��ָ��
	SOCKADDR_IN m_saUdpServ;
};

