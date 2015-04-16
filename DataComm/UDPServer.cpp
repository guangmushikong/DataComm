#include "StdAfx.h"
#include "UDPServer.h"

#define IP "127.0.0.1"
#define Port 16000

struct test {
 string str;
};

struct UdpHeartPack {
    char UDPData[16] ;
};

CUDPServer::CUDPServer(void)
{
}


CUDPServer::~CUDPServer(void)
{
	Exit();
}

bool CUDPServer::Init( int port )
{
	WSADATA wsaData;                                    //指向WinSocket信息结构的指针                     
	BOOL fBroadcast = TRUE;                             //用于setsockopt(),表示允许

	if(WSAStartup(MAKEWORD( 1, 1 ), &wsaData )!=0)     //进行WinSocket的初始化
	{
		//初始化失败返回-1
		printf("Can't initiates windows socket!\n");
		return -1;
	}
	//建立一个数据报类型的UDP套接字
	m_sockListener=socket(PF_INET,SOCK_DGRAM,0);     

	//setsockopt函数用于设置套接口选项
	//采用广播形式须将第三个参数设置为SO_BROADCAST
	setsockopt ( m_sockListener,SOL_SOCKET,SO_BROADCAST, (CHAR *)&fBroadcast, sizeof (BOOL) );

	//  参数设置，注意要将IP地址设为INADDR_BROADCAST，表示发送广播UDP数据报
	m_saUdpServ.sin_family = AF_INET;
	m_saUdpServ.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
	m_saUdpServ.sin_port = htons (port);  

	return 0;
}

void CUDPServer::SendData(const char* Buffer, int size)
{
	sendto ( m_sockListener,Buffer, size, 0, (SOCKADDR *) &m_saUdpServ, sizeof(SOCKADDR_IN));
	printf("%s\n",Buffer);                    //将要广播的数据串输出
}

void CUDPServer::Exit()
{
	//关闭监听socket
	closesocket(m_sockListener); 
	WSACleanup();
}