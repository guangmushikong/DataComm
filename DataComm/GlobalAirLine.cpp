#include "StdAfx.h"
#include "GlobalAirLine.h"

CGlobalAirLine * CGlobalAirLine::m_globalAirLine = NULL;
CURRENT_POINT CGlobalAirLine::m_CurrentPiontInfo;
CURRENT_POINT CGlobalAirLine::m_NextPiontInfo;
CGlobalAirLine::CGlobalAirLine(void)
{
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
void CGlobalAirLine::GetNextPiont(CURRENT_POINT &nextPT)
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
    EnterCriticalSection(&m_csCommunicationSync);  

	if( m_CurrentPiontInfo.lineIndex  ==lineIndex && m_CurrentPiontInfo.pintIndex )
	{
		m_CurrentPiontInfo.status = status;
	}
	    /** 离开临界区 */   
    LeaveCriticalSection(&m_csCommunicationSync);
}