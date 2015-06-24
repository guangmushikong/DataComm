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

///��ȡ���µ��������Ϣ
void CGlobalAirLine::GetCurrentPiont(CURRENT_POINT &curPT)
{
	curPT = m_CurrentPiontInfo;
}

	///�������µ��������Ϣ
void CGlobalAirLine::SetCurrentPiont(const CURRENT_POINT curPT)
{
	/** �ٽ������� */   
    EnterCriticalSection(&m_csCommunicationSync);  

	m_CurrentPiontInfo = curPT;

	    /** �뿪�ٽ��� */   
    LeaveCriticalSection(&m_csCommunicationSync);  
}

///��ȡǰ������Ϣ
void CGlobalAirLine::GetNextPiont(CURRENT_POINT &nextPT)
{
	nextPT = m_NextPiontInfo;
}

///����ǰ������Ϣ
void CGlobalAirLine::SetNextPiont(const CURRENT_POINT nextPT)
{
	/** �ٽ������� */   
    EnterCriticalSection(&m_csCommunicationSync);  

	m_NextPiontInfo = nextPT;

	    /** �뿪�ٽ��� */   
    LeaveCriticalSection(&m_csCommunicationSync);  
}

///�������������״̬
void CGlobalAirLine::SetPointStatus(int lineIndex, int pintIndex, bool status)
{
	/** �ٽ������� */   
    EnterCriticalSection(&m_csCommunicationSync);  

	if( m_CurrentPiontInfo.lineIndex  ==lineIndex && m_CurrentPiontInfo.pintIndex )
	{
		m_CurrentPiontInfo.status = status;
	}
	    /** �뿪�ٽ��� */   
    LeaveCriticalSection(&m_csCommunicationSync);
}