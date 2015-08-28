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
	WSADATA wsaData;                                    //ָ��WinSocket��Ϣ�ṹ��ָ��                     
	BOOL fBroadcast = TRUE;                             //����setsockopt(),��ʾ����

	if(WSAStartup(MAKEWORD( 1, 1 ), &wsaData )!=0)     //����WinSocket�ĳ�ʼ��
	{
		//��ʼ��ʧ�ܷ���-1
		printf("Can't initiates windows socket!\n");
		return -1;
	}
	//����һ�����ݱ����͵�UDP�׽���
	m_sockListener=socket(PF_INET,SOCK_DGRAM,0);     

	//setsockopt�������������׽ӿ�ѡ��
	//���ù㲥��ʽ�뽫��������������ΪSO_BROADCAST
	setsockopt ( m_sockListener,SOL_SOCKET,SO_BROADCAST, (CHAR *)&fBroadcast, sizeof (BOOL) );

	//  �������ã�ע��Ҫ��IP��ַ��ΪINADDR_BROADCAST����ʾ���͹㲥UDP���ݱ�
	m_saUdpServ.sin_family = AF_INET;
	m_saUdpServ.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
	m_saUdpServ.sin_port = htons (port);  

	///����server
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
	printf("%s\n",Buffer);                    //��Ҫ�㲥�����ݴ����
}

void CUDPServer::Exit()
{
	//�رռ���socket
	closesocket(m_sockListener); 
	WSACleanup();
}

UINT WINAPI CUDPServer::RecieveData(void* pParam)
{
	 /** �õ������ָ�� */   
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
				printf("�ع�ģʽ\n");
			}
			else if(  strcmp( buffer, "#1") == 0 )
			{
				CSystemParam::SetSystemStatus(SYS_ShootMiss);
				statusType = SYS_ShootMiss;
				printf("����ģʽ\n");
			}
			else if(  strcmp( buffer, "#2") == 0 )
			{
				CSystemParam::SetSystemStatus(SYS_Spiral);
				statusType = SYS_Spiral;
				printf("����ģʽ\n");
			}
			else if(  strcmp( buffer, "#3") == 0 )
			{
				CSystemParam::SetSystemStatus(SYS_BackAirport);
				statusType = SYS_BackAirport;
				printf("����ģʽ\n");
			}
			else if(  strcmp( buffer, "#4") == 0 )///������������
			{
				pUDPServe->SendData(pUDPServe->m_strConfig, sizeof(pUDPServe->m_strConfig));
				statusType = SYS_NULL;
				printf("������������\n");
			}
			CSqliteManger::GetInstance()->InsertSystemStatus(statusType);
		}
	} 
	return 0;
}

bool CUDPServer::OpenListenThread()  
{  
    /** ����߳��Ƿ��Ѿ������� */   
    if (m_hListenThread != INVALID_HANDLE_VALUE)  
    {  
        /** �߳��Ѿ����� */   
        return false;  
    }  
 
    /** �߳�ID */   
    UINT threadId;  
    /** �����������ݼ����߳� */   
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, RecieveData, this, 0, &threadId);  
    if (!m_hListenThread)  
    {  
        return false;  
    }  
 
    return true;  
} 