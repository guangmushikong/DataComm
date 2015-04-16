//////////////////////////////////////////////////////////////////////////  
/// COPYRIGHT NOTICE  
/// Copyright (c) 2009, 华中科技大学tickTick Group  （版权声明）  
/// All rights reserved.  
///   
/// @file    SerialPort.cpp    
/// @brief   串口通信类的实现文件  
///  
/// 本文件为串口通信类的实现代码  
///  
/// @version 1.0     
/// @author  卢俊    
/// @E-mail：lujun.hust@gmail.com  
/// @date    2010/03/19  
///   
///  
///  修订说明：  
//////////////////////////////////////////////////////////////////////////  
 
#include "StdAfx.h"  
#include "SerialPort.h"  
#include <process.h>  
#include <iostream>  
#include "LogFile.h"
#include "SqliteManger.h"
 
/** 线程退出标志 */   
bool CSerialPort::s_bExit = false;  
/** 当串口无数据时,sleep至下次查询间隔的时间,单位:秒 */   
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
 
    /** 临时变量,将制定参数转化为字符串形式,以构造DCB结构 */   
    char szDCBparam[50];  
    sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);  
 
    /** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */   
    if (!openPort(portNo))  
    {  
        return false;  
    }  
 
    /** 进入临界段 */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** 是否有错误发生 */   
    BOOL bIsSuccess = TRUE;  
 
    /** 在此可以设置输入输出的缓冲区大小,如果不设置,则系统会设置默认值.  
     *  自己设置缓冲区大小时,要注意设置稍大一些,避免缓冲区溢出  
     */ 
    /*if (bIsSuccess )  
    {  
        bIsSuccess = SetupComm(m_hComm,10,10);  
    }*/ 
 
    /** 设置串口的超时时间,均设为0,表示不使用超时限制 */ 
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
        // 将ANSI字符串转换为UNICODE字符串  
        DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, szDCBparam, -1, NULL, 0);  
        wchar_t *pwText = new wchar_t[dwNum] ;  
        if (!MultiByteToWideChar (CP_ACP, 0, szDCBparam, -1, pwText, dwNum))  
        {  
            bIsSuccess = TRUE;  
        }  
 
        /** 获取当前串口配置参数,并且构造串口DCB参数 */   
        bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb) ;  
        /** 开启RTS flow控制 */   
        dcb.fRtsControl = RTS_CONTROL_ENABLE;   
 
        /** 释放内存空间 */   
        delete [] pwText;  
    }  
 
    if ( bIsSuccess )  
    {  
        /** 使用DCB参数配置串口状态 */   
        bIsSuccess = SetCommState(m_hComm, &dcb);  
    }  
          
    /**  清空串口缓冲区 */ 
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  
 
    /** 离开临界段 */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return bIsSuccess==TRUE;  
}  
 
bool CSerialPort::InitPort( UINT portNo ,const LPDCB& plDCB )  
{  
    /** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */   
    if (!openPort(portNo))  
    {  
        return false;  
    }  
      
    /** 进入临界段 */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** 配置串口参数 */   
    if (!SetCommState(m_hComm, plDCB))  
    {  
        return false;  
    }  
 
    /**  清空串口缓冲区 */ 
    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  
 
    /** 离开临界段 */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return true;  
}  
 
void CSerialPort::ClosePort()  
{  
    /** 如果有串口被打开，关闭它 */ 
    if( m_hComm != INVALID_HANDLE_VALUE )  
    {  
        CloseHandle( m_hComm );  
        m_hComm = INVALID_HANDLE_VALUE;  
    }  
}  
 
bool CSerialPort::openPort( UINT portNo )  
{  
    /** 进入临界段 */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** 把串口的编号转换为设备名 */   
    char szPort[50];  
    sprintf_s(szPort, "COM%d", portNo);  
 
    /** 打开指定的串口 */   
    m_hComm = CreateFileA(szPort,  /** 设备名,COM1,COM2等 */   
              GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */     
              0,                            /** 共享模式,0表示不共享 */   
              NULL,                         /** 安全性设置,一般使用NULL */   
              OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */   
              0,      
              0);      
 
    /** 如果打开失败，释放资源并返回 */   
    if (m_hComm == INVALID_HANDLE_VALUE)  
    {  
        LeaveCriticalSection(&m_csCommunicationSync);  
        return false;  
    }  
 
    /** 退出临界区 */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return true;  
}  
 
bool CSerialPort::OpenListenThread()  
{  
    /** 检测线程是否已经开启了 */   
    if (m_hListenThread != INVALID_HANDLE_VALUE)  
    {  
        /** 线程已经开启 */   
        return false;  
    }  
 
    s_bExit = false;  
    /** 线程ID */   
    UINT threadId;  
    /** 开启串口数据监听线程 */   
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);  
    if (!m_hListenThread)  
    {  
        return false;  
    }  
    /** 设置线程的优先级,高于普通线程 */   
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
        /** 通知线程退出 */   
        s_bExit = true;  
 
        /** 等待线程退出 */   
        Sleep(10);  
 
        /** 置线程句柄无效 */   
        CloseHandle( m_hListenThread );  
        m_hListenThread = INVALID_HANDLE_VALUE;  
    }  
    return true;  
}  
 
UINT CSerialPort::GetBytesInCOM()  
{  
    DWORD dwError = 0;  /** 错误码 */   
    COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */   
    memset(&comstat, 0, sizeof(COMSTAT));  
 
    UINT BytesInQue = 0;  
    /** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */   
    if ( ClearCommError(m_hComm, &dwError, &comstat) )  
    {  
        BytesInQue = comstat.cbInQue; /** 获取在输入缓冲区中的字节数 */   
    }  
 
    return BytesInQue;  
}  
 
UINT WINAPI CSerialPort::ListenThread( void* pParam )  
{  
    /** 得到本类的指针 */   
    CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);  
 
    // 线程循环,轮询方式读取串口数据  
    while (!pSerialPort->s_bExit)   
    {  
        UINT BytesInQue = pSerialPort->GetBytesInCOM();  
        /** 如果串口输入缓冲区中无数据,则休息一会再查询 */   
        if ( BytesInQue == 0 )  
        {  
            Sleep(SLEEP_TIME_INTERVAL);  
            continue;  
        }  

        /** 读取输入缓冲区中的数据 */ 
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
					string log = "接收到最简位置数据GPRMC：";
			        char cLOG[180];
					sprintf(cLOG,"经度%f 纬度%f 高程%f 速度%f 方位角%f", Msg.body.position_info.pos.lon,Msg.body.position_info.pos.lat,Msg.body.position_info.pos.high,
						Msg.body.position_info.vel,Msg.body.position_info.az);
					log += cLOG;
					//	CLogFile *pFile;
	                //   pFile->GetInstance()->WriteLog("系统启动！", 10);
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
 
    /** 临界区保护 */   
    EnterCriticalSection(&m_csCommunicationSync);  
 
    /** 向缓冲区写入指定量的数据 */   
    bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);  
    if (!bResult)    
    {  
        DWORD dwError = GetLastError();  
        /** 清空串口缓冲区 */   
        PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);  
        LeaveCriticalSection(&m_csCommunicationSync);  
 
        return false;  
    }  
 
    /** 离开临界区 */   
    LeaveCriticalSection(&m_csCommunicationSync);  
 
    return true;  
}  

/**
* @brief: 拼包。
* @description:	
* @param: pBuffer 数据缓存
* @param: size 数据长度
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

    ///计算校验位///////////////////////////////////
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