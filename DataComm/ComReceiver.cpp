#include "StdAfx.h"
#include "ComReceiver.h"
#include <process.h>  

/** 线程退出标志 */   
bool CComReceiver::s_bExit = false; 
CComReceiver::CComReceiver(void)
{
	m_pContainer = new CDataContainer<string*>();
	m_hEvtsDPThread[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvtsDPThread[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hDataProcessThread = INVALID_HANDLE_VALUE;
}


CComReceiver::~CComReceiver(void)
{
}

UINT CComReceiver::DPThdImp( LPVOID pParam )
{	
	string *pstrMsg;
	while (TRUE)
	{
		DWORD ret = WaitForMultipleObjects(2, m_hEvtsDPThread, FALSE, 10);
		
		switch (ret)
		{
		case WAIT_OBJECT_0:
			{
				ResetEvent(m_hEvtsDPThread[0]);
				return 0;
			}
		case WAIT_OBJECT_0 + 1:
			{							
				ResetEvent(m_hEvtsDPThread[1]);
				
				while (m_pContainer->GetData(pstrMsg))
				{
					COMM_MESSAGE Msg;
					FormatTrans(*pstrMsg, &Msg);
					Notify( (char*)&Msg, sizeof(COMM_MESSAGE));
				}
			}
		}
	}
}

UINT WINAPI CComReceiver::DataProcessThread( void* pParam )  
{  
    /** 得到本类的指针 */   
    CComReceiver *pRev = reinterpret_cast<CComReceiver*>(pParam);
	return pRev->DPThdImp(pParam);
}

bool CComReceiver::OpenDataProcessThread()  
{ 
	 /** 检测线程是否已经开启了 */   
    if (m_hDataProcessThread != INVALID_HANDLE_VALUE)  
    {  
        /** 线程已经开启 */   
        return false;  
    }  
 
    s_bExit = false;  
    /** 线程ID */   
    UINT threadId;  
    /** 开启串口数据监听线程 */   
    m_hDataProcessThread = (HANDLE)_beginthreadex(NULL, 0, DataProcessThread, this, 0, &threadId);  
    if (!m_hDataProcessThread)  
    {  
        return false;  
    }   
    return true;  
}  

void CComReceiver::DataUpdate( char *pBuffer, int size )
{
	if ( size > MAX_BUFFER_SIZE || size < 6 )
	{
		return;
	}	
	
	string* lpData = new string;
	lpData->append( pBuffer, size);
	m_pContainer->PushData( lpData );

	SetEvent(m_hEvtsDPThread[1]);		
}

int CComReceiver::FormatTrans(const string &data, COMM_MESSAGE *pMsg)
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
		return 1;
	}
	else if (strncmp(pData, "$GPVTG", 6) == 0)
	{
		pMsg->msgtype = MSG_GPVTG;
		m_dataProcess.UnPackGPVTG(data, &pMsg->body.velocity);
		return 1;
	}
	return 1;
}

void CComReceiver::AddObserver( IDataUpdate *pObserver )
{
	m_vecObservers.push_back(pObserver);	
}

void CComReceiver::RemoveObserver( IDataUpdate *pObserver )
{
	vector<IDataUpdate*>::iterator iter;

	iter = find(m_vecObservers.begin(), m_vecObservers.end(), pObserver);	

	if (iter != m_vecObservers.end())
		m_vecObservers.erase(iter);
}

void CComReceiver::Notify(char *pBuffer, int size)
{
	for (int i = 0; i < m_vecObservers.size(); i++)
	{
		m_vecObservers[i]->DataUpdate(pBuffer, size);
	}
}