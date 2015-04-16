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
	///初始化UDP
	bool Init( int port );

	///发送数据
	void SendData(const char* Buffer, int size);

	///退出UDP
	void Exit();
private:
	//创建套接字
	SOCKET m_sockListener;  

	//指向通信对象的结构体指针
	SOCKADDR_IN m_saUdpServ;
};

