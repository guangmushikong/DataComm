#include "StdAfx.h"
#include "GlobalAirLine.h"

CGlobalAirLine * CGlobalAirLine::m_globalAirLine = NULL;
CURRENT_POINT CGlobalAirLine::m_CurrentPiontInfo;
CURRENT_POINT CGlobalAirLine::m_NextPiontInfo;
list<int>  CGlobalAirLine::m_pointList;
std::map<int, int> CGlobalAirLine::m_ExposureLine;

CGlobalAirLine::CGlobalAirLine(void)
{
	InitializeCriticalSectionAndSpinCount(&m_csCommunicationSync, 0x80000FA0);
}

void CGlobalAirLine::Release()
{
	if(m_globalAirLine != NULL)
	{
		delete m_globalAirLine;
		m_globalAirLine = NULL;
	}
}

CGlobalAirLine * CGlobalAirLine::GetInstance()
{
	if( m_globalAirLine == NULL )
	{
		m_globalAirLine = new CGlobalAirLine();
	}

	return m_globalAirLine;
}

///获取最新的拍摄点信息
void CGlobalAirLine::GetCurrentPiont(CURRENT_POINT &curPT)
{
	curPT = m_CurrentPiontInfo;
}

	///设置最新的拍摄点信息
void CGlobalAirLine::SetCurrentPiont(const CURRENT_POINT curPT)
{
	/** 临界区保护 */   
    EnterCriticalSection(&m_csCommunicationSync);  

	m_CurrentPiontInfo = curPT;

	    /** 离开临界区 */   
    LeaveCriticalSection(&m_csCommunicationSync);  
}

///获取前方点信息
void CGlobalAirLine::GetNextPiont(CURRENT_POINT &nextPT )
{
	nextPT = m_NextPiontInfo;
}

///设置前方点信息
void CGlobalAirLine::SetNextPiont(const CURRENT_POINT nextPT)
{
	/** 临界区保护 */   
    EnterCriticalSection(&m_csCommunicationSync);  

	m_NextPiontInfo = nextPT;

	    /** 离开临界区 */   
    LeaveCriticalSection(&m_csCommunicationSync);  
}

///设置拍摄点拍摄状态
void CGlobalAirLine::SetPointStatus(int lineIndex, int pintIndex, bool status)
{
	/** 临界区保护 */   
//    EnterCriticalSection(&m_csCommunicationSync);  

	if( m_CurrentPiontInfo.lineIndex  ==lineIndex && m_CurrentPiontInfo.pintIndex )
	{
		m_CurrentPiontInfo.status = status;
	}
	    /** 离开临界区 */   
//    LeaveCriticalSection(&m_csCommunicationSync);
}

///添加已拍摄拍摄点
void CGlobalAirLine::SetExposurePoint(int lineIndex, int pointIndex )
{
	/** 临界区保护 */   
    EnterCriticalSection(&m_csCommunicationSync);  

	if( lineIndex > 0 && pointIndex > 0 )
	{
		int newint = JoinInt(lineIndex, pointIndex );
		m_pointList.push_back(newint);

		///添加航线已曝光点个数
		std::map<int, int>::iterator itRate = m_ExposureLine.find( lineIndex);
		if ( itRate != m_ExposureLine.end() )
		{
			itRate->second++;
		}
		else
		{
			m_ExposureLine.insert(make_pair(lineIndex, 1));
		}
		
	}

	 /** 离开临界区 */   
    LeaveCriticalSection(&m_csCommunicationSync);
}

///获取曝光点是否曝光状态
bool CGlobalAirLine::GetExposurePointStatus(int lineIndex, int pointIndex)
{
	if( lineIndex > 0 && pointIndex > 0 )
	{
		int newint = JoinInt(lineIndex, pointIndex );
		std::list<int>::iterator iter = m_pointList.begin();
		for( ; iter != m_pointList.end(); iter++ )
		{
			if( *iter == newint )
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}

int CGlobalAirLine::GetExposureRateLine(const int lineIndex)
{
	if( lineIndex > 0  )
	{
		///获取航线已曝光点个数
		std::map<int, int>::iterator itRate = m_ExposureLine.find( lineIndex);
		if ( itRate != m_ExposureLine.end() )
		{
			return m_ExposureLine.find(lineIndex)->second;
		}		
	}
	return 0;
}

int CGlobalAirLine::JoinInt(int front,int back)
{ 
	int nResult = 0; 
	char sBuf[20] = {'\0'};
	sprintf(sBuf, "%d%d", front, back);
	nResult = atoi(sBuf);
	return nResult;
}
