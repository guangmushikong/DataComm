#include "StdAfx.h"
#include "UDPServer.h"
#include <process.h> 
#include "SqliteManger.h"

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
	m_bExitFlag = false;
	m_hListenThread = INVALID_HANDLE_VALUE;

	EXPOSURE_PARAM  param;
	CSystemParam::GetExposurParam(param);
	memset(m_strConfig, 0, sizeof(m_strConfig));
	sprintf(m_strConfig, "status:config,distance:%f,height:%f", param.distan, param.hAllow);
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

	///监听server
	sockaddr_in local;

	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_family =  AF_INET;
	local.sin_port = htons(port);

	m_ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int iR = bind(m_ServerSocket, (struct sockaddr *)&local, sizeof(sockaddr_in));

	m_lastSYSStatus = SYS_Spiral;
	OpenListenThread();
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

UINT WINAPI CUDPServer::RecieveData(void* pParam)
{
	 /** 得到本类的指针 */   
    CUDPServer *pUDPServe = reinterpret_cast<CUDPServer*>(pParam);  
	sockaddr_in from;
    int fromlen = sizeof(sockaddr_in);
	while(1)
	{
		char buffer[512] = "\0";

		printf("waiting message form client:");
		if(SOCKET_ERROR != recvfrom(pUDPServe->m_ServerSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &fromlen ))
		{ 
			if( pUDPServe->m_bExitFlag )
			{
				printf("client quit!\n");
				break;
			}
			if( strlen(buffer) > 3)
			{
				continue;
			}
			System_Status statusType;
			printf("recieve message form %s -- %s \n",	inet_ntoa(from.sin_addr), buffer);
			if(  strcmp( buffer, "#0") == 0 )
			{
				CSystemParam::SetSystemStatus(SYS_Normal);
				statusType = SYS_Normal;
				printf("曝光模式\n");
			}
			else if(  strcmp( buffer, "#1") == 0 )
			{
				CSystemParam::SetSystemStatus(SYS_ShootMiss);
				statusType = SYS_ShootMiss;
				printf("补拍模式\n");
			}
			else if(  strcmp( buffer, "#2") == 0 )
			{
				CSystemParam::SetSystemStatus(SYS_Spiral);
				statusType = SYS_Spiral;
				printf("盘旋模式\n");
			}
			else if(  strcmp( buffer, "#3") == 0 )
			{
				CSystemParam::SetSystemStatus(SYS_BackAirport);
				statusType = SYS_BackAirport;
				printf("返航模式\n");
			}
			else if(  strcmp( buffer, "#4") == 0 )///申请配置命令
			{
				pUDPServe->SendData(pUDPServe->m_strConfig, sizeof(pUDPServe->m_strConfig));
				statusType = SYS_NULL;
				printf("申请配置命令\n");
			}
			CSqliteManger::GetInstance()->InsertSystemStatus(statusType);
		}
	} 
	return 0;
}

bool CUDPServer::OpenListenThread()  
{  
    /** 检测线程是否已经开启了 */   
    if (m_hListenThread != INVALID_HANDLE_VALUE)  
    {  
        /** 线程已经开启 */   
        return false;  
    }  
 
    /** 线程ID */   
    UINT threadId;  
    /** 开启串口数据监听线程 */   
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, RecieveData, this, 0, &threadId);  
    if (!m_hListenThread)  
    {  
        return false;  
    }  
 
    return true;  
} 