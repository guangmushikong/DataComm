//////////////////////////////////////////////////////////////////////////  
/// COPYRIGHT NOTICE  
/// Copyright (c) 2009, ���пƼ���ѧtickTick Group  ����Ȩ������  
/// All rights reserved.  
///   
/// @file    SerialPort.cpp    
/// @brief   ����ͨ�����ʵ���ļ�  
///  
/// ���ļ�Ϊ����ͨ�����ʵ�ִ���  
///  
/// @version 1.0     
/// @author  ¬��    
/// @E-mail��lujun.hust@gmail.com  
/// @date    2010/03/19  
///   
///  
///  �޶�˵����  
//////////////////////////////////////////////////////////////////////////  
 
#include "StdAfx.h"  
#include "SerialPort.h"  
#include <process.h>  
#include <iostream>  
#include "LogFile.h"
#include "SqliteManger.h"
 
/** �߳��˳���־ */   
bool CSerialPort::s_bExit = false;  
/** ������������ʱ,sleep���´β�ѯ�����ʱ��,��λ:�� */   
const UINT SLEEP_TIME_INTERVAL = 5;  
 
CSerialPort::CSerialPort(void)  
: m_hListenThread(INVALID_HANDLE_VALUE)  
{  
    m_hComm = INVALID_HANDLE_VALUE;  
    m_hListenThread = INVALID_HANDLE_VALUE;  
 
    InitializeCriticalSection(&m_csCommunicationSync);  
	m_iBufferLen = 0;
	m_high      = 0.0;
 
}  
 
CSerialPort::~CSerialPort(void)  
{  
    CloseListenTread();  
    ClosePort();  
    DeleteCriticalSection(&m_csCommunicationSync);  
}  
 
bool CSerialPort::InitPort( UINT portNo /*= 1*/,UINT baud /*= CBR_9600*/,char parity /*= 'N'*/,  
                            UINT databits /*= 8*/, UINT stopsbits /*= 1*/,DWORD dwCommEvents /*= EV_RXCHAR*/ )  
{  
 
    /** ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */   
    char szDCBparam[50];  
    sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);  
 
    /** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */   
    if (!openPort(portNo))  
    {  
        return false;  
    }  
 
    /** �����ٽ�� */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** �Ƿ��д����� */   
    BOOL bIsSuccess = TRUE;  
 
    /** �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.  
     *  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������  
     */ 
    /*if (bIsSuccess )  
    {  
        bIsSuccess = SetupComm(m_hComm,10,10);  
    }*/ 
 
    /** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */ 
    COMMTIMEOUTS  CommTimeouts;  
    CommTimeouts.ReadIntervalTimeout         = 0;  
    CommTimeouts.ReadTotalTimeoutMultiplier  = 0;  
    CommTimeouts.ReadTotalTimeoutConstant    = 0;  
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;  
    CommTimeouts.WriteTotalTimeoutConstant   = 0;   
    if ( bIsSuccess)  
    {  
        bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);  
    }  
 
    DCB  dcb;  
    if ( bIsSuccess )  
    {  
        // ��ANSI�ַ���ת��ΪUNICODE�ַ���  
        DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, szDCBparam, -1, NULL, 0);  
        wchar_t *pwText = new wchar_t[dwNum] ;  
        if (!MultiByteToWideChar (CP_ACP, 0, szDCBparam, -1, pwText, dwNum))  
        {  
            bIsSuccess = TRUE;  
        }  
 
        /** ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */   
        bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb) ;  
        /** ����RTS flow���� */   
        dcb.fRtsControl = RTS_CONTROL_ENABLE;   
 
        /** �ͷ��ڴ�ռ� */   
        delete [] pwText;  
    }  
 
    if ( bIsSuccess )  
    {  
        /** ʹ��DCB�������ô���״̬ */   
        bIsSuccess = SetCommState(m_hComm, &dcb);  
    }  
          
    /**  ��մ��ڻ����� */ 
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  
 
    /** �뿪�ٽ�� */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return bIsSuccess==TRUE;  
}  
 
bool CSerialPort::InitPort( UINT portNo ,const LPDCB& plDCB )  
{  
    /** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */   
    if (!openPort(portNo))  
    {  
        return false;  
    }  
      
    /** �����ٽ�� */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** ���ô��ڲ��� */   
    if (!SetCommState(m_hComm, plDCB))  
    {  
        return false;  
    }  
 
    /**  ��մ��ڻ����� */ 
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  
 
    /** �뿪�ٽ�� */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return true;  
}  
 
void CSerialPort::ClosePort()  
{  
    /** ����д��ڱ��򿪣��ر��� */ 
    if( m_hComm != INVALID_HANDLE_VALUE )  
    {  
        CloseHandle( m_hComm );  
        m_hComm = INVALID_HANDLE_VALUE;  
    }  
}  
 
bool CSerialPort::openPort( UINT portNo )  
{  
    /** �����ٽ�� */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** �Ѵ��ڵı��ת��Ϊ�豸�� */   
    char szPort[50];  
    sprintf_s(szPort, "COM%d", portNo);  
 
    /** ��ָ���Ĵ��� */   
    m_hComm = CreateFileA(szPort,  /** �豸��,COM1,COM2�� */   
              GENERIC_READ | GENERIC_WRITE, /** ����ģʽ,��ͬʱ��д */     
              0,                            /** ����ģʽ,0��ʾ������ */   
              NULL,                         /** ��ȫ������,һ��ʹ��NULL */   
              OPEN_EXISTING,                /** �ò�����ʾ�豸�������,���򴴽�ʧ�� */   
              0,      
              0);      
 
    /** �����ʧ�ܣ��ͷ���Դ������ */   
    if (m_hComm == INVALID_HANDLE_VALUE)  
    {  
        LeaveCriticalSection(&m_csCommunicationSync);  
        return false;  
    }  
 
    /** �˳��ٽ��� */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return true;  
}  
 
bool CSerialPort::OpenListenThread()  
{  
    /** ����߳��Ƿ��Ѿ������� */   
    if (m_hListenThread != INVALID_HANDLE_VALUE)  
    {  
        /** �߳��Ѿ����� */   
        return false;  
    }  
 
    s_bExit = false;  
    /** �߳�ID */   
    UINT threadId;  
    /** �����������ݼ����߳� */   
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);  
    if (!m_hListenThread)  
    {  
        return false;  
    }  
    /** �����̵߳����ȼ�,������ͨ�߳� */   
    if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))  
    {  
        return false;  
    }  
 
    return true;  
}  
 
bool CSerialPort::CloseListenTread()  
{     
    if (m_hListenThread != INVALID_HANDLE_VALUE)  
    {  
        /** ֪ͨ�߳��˳� */   
        s_bExit = true;  
 
        /** �ȴ��߳��˳� */   
        Sleep(10);  
 
        /** ���߳̾����Ч */   
        CloseHandle( m_hListenThread );  
        m_hListenThread = INVALID_HANDLE_VALUE;  
    }  
    return true;  
}  
 
UINT CSerialPort::GetBytesInCOM()  
{  
    DWORD dwError = 0;  /** ������ */   
    COMSTAT  comstat;   /** COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */   
    memset(&comstat, 0, sizeof(COMSTAT));  
 
    UINT BytesInQue = 0;  
    /** �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ�����Ĵ����־ */   
    if ( ClearCommError(m_hComm, &dwError, &comstat) )  
    {  
        BytesInQue = comstat.cbInQue; /** ��ȡ�����뻺�����е��ֽ��� */   
    }  
 
    return BytesInQue;  
}  
 
UINT WINAPI CSerialPort::ListenThread( void* pParam )  
{  
    /** �õ������ָ�� */   
    CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);  
 
    // �߳�ѭ��,��ѯ��ʽ��ȡ��������  
    while (!pSerialPort->s_bExit)   
    {  
        UINT BytesInQue = pSerialPort->GetBytesInCOM();  
        /** ����������뻺������������,����Ϣһ���ٲ�ѯ */   
        if ( BytesInQue == 0 )  
        {  
            Sleep(SLEEP_TIME_INTERVAL);  
            continue;  
        }  

        /** ��ȡ���뻺�����е����� */ 
		static char cRecved[1024];
	    memset(cRecved,0,1024);
		DWORD len;
		if(pSerialPort->ReadChar((char*)cRecved,len) == TRUE)  
        {  
            //std::cout << cRecved ; 
			COMM_MESSAGE Msg;
			memset(pSerialPort->m_cRecved, 0, 1024);
			int iBinLen = 0;
			pSerialPort->DataPackage(cRecved, len, pSerialPort->m_cRecved, iBinLen);
			if( iBinLen > 0)
			{
				//pSerialPort->FormatTrans(cRecved, &Msg);
			    pSerialPort->FormatTrans(pSerialPort->m_cRecved, &Msg);
				pSerialPort->Notify((char*)&Msg, sizeof(COMM_MESSAGE));
				if( Msg.msgtype == MSG_GPRMC)
				{
					Msg.body.position_info.pos.high = pSerialPort->m_high;
					string log = "���յ����λ������GPRMC��";
			        char cLOG[180];
					sprintf(cLOG,"����%f γ��%f �߳�%f �ٶ�%f ��λ��%f", Msg.body.position_info.pos.lon,Msg.body.position_info.pos.lat,Msg.body.position_info.pos.high,
						Msg.body.position_info.vel,Msg.body.position_info.az);
					log += cLOG;
					//	CLogFile *pFile;
	                //   pFile->GetInstance()->WriteLog("ϵͳ������", 10);
					pSerialPort->m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());
//					CSqliteManger::GetInstance()->InsertPosition(Msg.body.position_info);
				}
				pSerialPort->Notify((char*)&Msg, sizeof(COMM_MESSAGE));
			}

            continue;  
        }  
    }  
 
    return 0;  
}  

BOOL CSerialPort::ReadChar( char *cRecved, DWORD &dwBytesRead )  
{   
	if(m_hComm == INVALID_HANDLE_VALUE)  
	{  
		return FALSE;  
	}  

	dwBytesRead = 1024;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	memset(&m_osRead,0,sizeof(OVERLAPPED));
	m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	ClearCommError(m_hComm, &dwErrorFlags, &ComStat);
	dwBytesRead = min(dwBytesRead,(DWORD)ComStat.cbInQue);
	if( dwBytesRead <= 0 )
	{
		return FALSE;
	}
	return ReadFile(m_hComm, cRecved, dwBytesRead, &dwBytesRead ,&m_osRead);
}  
 
bool CSerialPort::WriteData( unsigned char* pData, unsigned int length )  
{  
    BOOL   bResult  = true;  
    DWORD  BytesToSend = 0;  
    if(m_hComm == INVALID_HANDLE_VALUE)  
    {  
        return false;  
    }  
 
    /** �ٽ������� */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** �򻺳���д��ָ���������� */   
    bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);  
    if (!bResult)    
    {  
        DWORD dwError = GetLastError();  
        /** ��մ��ڻ����� */   
        PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);  
        LeaveCriticalSection(&m_csCommunicationSync);  
 
        return false;  
    }  
 
    /** �뿪�ٽ��� */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return true;  
}  

/**
* @brief: ƴ����
* @description:	
* @param: pBuffer ���ݻ���
* @param: size ���ݳ���
* @return: N/A
*/
void CSerialPort::DataPackage( const char *pBuffer, const int size,  char *pOutBuffer, int &iBinLen)
{
	if (m_iBufferLen + size > 8192)
		m_iBufferLen = 0;
	
	memcpy(m_strBuffer + m_iBufferLen, pBuffer, size);
	m_iBufferLen += size;
	int iCurLen = m_iBufferLen;
	
	int start = -1;
	for (int i = 0; i < iCurLen; i++)
	{
		if ( (byte)m_strBuffer[i] == '$' )
		{
			start = i;
		}
		else if(start != -1 && (byte)m_strBuffer[i] == '\r' && (byte)m_strBuffer[i+1] == '\n')
		{
			if(start != 0 )
			{
			    memcpy(m_strBuffer, m_strBuffer + start, m_iBufferLen - start);
			    m_iBufferLen = m_iBufferLen - start;
			}

			iBinLen = i - start;
			memcpy(pOutBuffer, m_strBuffer, iBinLen);		

			memcpy(m_strBuffer, m_strBuffer + iBinLen, m_iBufferLen - iBinLen);
			m_iBufferLen = m_iBufferLen - iBinLen;

			//		SetEvent(m_hEvtsDPThread[1]);

			break;

		}

		if (i >= iCurLen)
		{
			m_iBufferLen = 0;
		}
	}
	
}

int CSerialPort::FormatTrans(const string &data, COMM_MESSAGE *pMsg)
{
    const char *pData = data.data();

    ///����У��λ///////////////////////////////////
//    BYTE checknum = CDataProcess::ComCheckSum((BYTE*)pData, data.length()-1 );
//    if (checknum != (BYTE)pData[data.length() - 1])
//	  {
//			return 0;
//	  }

	if (strncmp(data.data(), "$GPGGA", 6) == 0)
	{
		pMsg->msgtype = MSG_GPGGA;
		m_dataProcess.UnPackGPGGA(data, &pMsg->body.position);
		m_high = pMsg->body.position.pos.high;
		return 1;
	}
	else if (strncmp(pData, "$GPVTG", 6) == 0)
	{
		pMsg->msgtype = MSG_GPVTG;
		m_dataProcess.UnPackGPVTG(data, &pMsg->body.velocity);
		return 1;
	}
	else if (strncmp(pData, "$GPRMC", 6) == 0)
	{
		pMsg->msgtype = MSG_GPRMC;
		m_dataProcess.UnPackGPRMC(data, &pMsg->body.position_info);
		return 1;
	}
	
	return 1;
}
void CSerialPort::AddObserver( IDataUpdate *pObserver )
{
	m_vecObservers.push_back(pObserver);	
}

void CSerialPort::RemoveObserver( IDataUpdate *pObserver )
{
	vector<IDataUpdate*>::iterator iter;

	iter = find(m_vecObservers.begin(), m_vecObservers.end(), pObserver);	

	if (iter != m_vecObservers.end())
		m_vecObservers.erase(iter);
}

void CSerialPort::Notify(char *pBuffer, int size)
{
	for (int i = 0; i < m_vecObservers.size(); i++)
	{
		m_vecObservers[i]->DataUpdate(pBuffer, size);
	}
}