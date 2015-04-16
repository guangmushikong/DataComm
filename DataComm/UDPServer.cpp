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