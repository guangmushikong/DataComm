#include "StdAfx.h"
#include "MatchAirLinePoint.h"
#include "datadefine.h"
#include <process.h> 

/** 线程退出标志 */   
bool CMatchAirLinePoint::s_bExitMatch = false; 
CMatchAirLinePoint::CMatchAirLinePoint(void)
{
	m_pContainer = new CDataContainer<string*>();
	m_hEvtsMatchThread = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pGlobalAirLine = CGlobalAirLine::GetInstance();
	m_hMachProcessThread = INVALID_HANDLE_VALUE;
	m_sysParam.IniSysParam();
/*
	COMM_MESSAGE Msg;
	Msg.msgtype = MSG_GPRMC;
	Msg.body.position_info.az = 22;
	Msg.body.position_info.vel = 17.964400;
	Msg.body.position_info.pos.lon = 113.955038;
	Msg.body.position_info.pos.lat = 35.440032;
	Msg.body.position_info.pos.high = 111.3;

	DataUpdate((char*)&Msg,sizeof(COMM_MESSAGE));*/
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
						//MatchCurrentAirLinePT(commMsg->body.position.pos);
					}
					else if( commMsg->msgtype == MSG_GPVTG)///速度航向信息
					{

					}
					if(commMsg->msgtype == MSG_GPRMC)///位置信息
					{
						MatchCurrentAirLinePT(commMsg->body.position_info);
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

void CMatchAirLinePoint::MatchCurrentAirLinePT(const GPRMC position)
{
	//获取航迹点信息
//	std::list<GuidancePoint*>& lsGPs;
//	m_sysParam.GetAirLinePTList(lsGPs);
///获取当前拍摄点信息
//	CURRENT_POINT pt;
//	m_pGlobalAirLine->GetCurrentPiont(pt);
	///更新当前拍摄点（换点时）
//  m_pGlobalAirLine->SetCurrentPiont(\* *\);
	//GuidancePoint point = m_sysParam.getMatchingPointFromGPs(position);
	//CURRENT_POINT cP;
	//cP.lineIndex = point.nLineNum;
	//cP.pintIndex = point.nPointNum;
	//cP.position.high = point.point.high;
	//cP.position.lat  = point.point.lat;
	//cP.position.lon  = point.point.lon;
	//cP.airline_az = .0;
	//m_pGlobalAirLine->SetCurrentPiont(cP);
	GuidancePoint gp;
	CURRENT_POINT cp;
	CSystemParam::getMatchedGP(gp, position);
	cp.position = gp.point;
	cp.lineIndex = gp.nLineIndex;
	cp.pintIndex = gp.nPointIndex;
	cp.distanceMatchFlag = gp.getDistanceMatchedStatus();
	cp.airlineMatchFlag = gp.getAirLineMatchedStatus();
	cp.headingMatchFlag = gp.getHeadingMatchedStatus();
	m_pGlobalAirLine->SetCurrentPiont(cp);	
}