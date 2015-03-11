#include "StdAfx.h"
#include "MatchAirLinePoint.h"
#include <process.h> 

/** 线程退出标志 */   
bool CMatchAirLinePoint::s_bExitMatch = false; 
CMatchAirLinePoint::CMatchAirLinePoint(void)
{
	m_pContainer = new CDataContainer<string*>();
	m_hEvtsMatchThread = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pGlobalAirLine = CGlobalAirLine::GetInstance();
	m_hMachProcessThread = INVALID_HANDLE_VALUE;
}


CMatchAirLinePoint::~CMatchAirLinePoint(void)
{
}

void CMatchAirLinePoint::DataUpdate( char *pBuffer, int size )
{	
	if ( size != sizeof(COMM_MESSAGE) )
	{
		return;
	}	
	
	string* lpData = new string;
	lpData->append( pBuffer, size);
	COMM_MESSAGE msg = *(COMM_MESSAGE*)pBuffer;
	m_pContainer->PushData( lpData );

	SetEvent(m_hEvtsMatchThread);	
}

UINT CMatchAirLinePoint::DPThdImp( LPVOID pParam )
{	
	string *pstrMsg;
	while ( !s_bExitMatch  )
	{
		DWORD ret = WaitForSingleObject(m_hEvtsMatchThread, -1 );

		switch (ret)
		{
		case WAIT_OBJECT_0:
			{						
				ResetEvent(m_hEvtsMatchThread);
				
				while (m_pContainer->GetData(pstrMsg))
				{
					COMM_MESSAGE *commMsg = (COMM_MESSAGE* )pstrMsg->c_str();
					if(commMsg->msgtype == MSG_GPGGA)///位置信息
					{
						MatchCurrentAirLinePT(commMsg->body.position.pos);
					}
					else if( commMsg->msgtype == MSG_GPVTG)///速度航向信息
					{

					}
				}
			}
		}
	}
	return 1;
}

UINT WINAPI CMatchAirLinePoint::MatchProcessThread( void* pParam )  
{  
    /** 得到本类的指针 */   
    CMatchAirLinePoint *pMatch = reinterpret_cast<CMatchAirLinePoint*>(pParam);
	return pMatch->DPThdImp(pParam);
}

bool CMatchAirLinePoint::OpenMatchProcessThread()  
{ 
	 /** 检测线程是否已经开启了 */   
    if (m_hMachProcessThread != INVALID_HANDLE_VALUE)  
    {  
        /** 线程已经开启 */   
        return false;  
    }  
 
    s_bExitMatch = false;  
    /** 线程ID */   
    UINT threadId;  
    /** 开启数据接受线程 */   
    m_hMachProcessThread = (HANDLE)_beginthreadex(NULL, 0, MatchProcessThread, this, 0, &threadId);  
    if (!m_hMachProcessThread)  
    {  
        return false;  
    }   
    return true;  
}  

void CMatchAirLinePoint::MatchCurrentAirLinePT(const COORDINATE position)
{
	//获取航迹点信息
//	std::list<GuidancePoint*>& lsGPs;
//	m_sysParam.GetAirLinePTList(lsGPs);

///获取当前拍摄点信息
//	CURRENT_POINT pt;
//	m_pGlobalAirLine->GetCurrentPiont(pt);

	///更新当前拍摄点（换点时）
//  m_pGlobalAirLine->SetCurrentPiont(\* *\);
}