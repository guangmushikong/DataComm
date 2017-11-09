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
void CGlobalAirLine::GetNextPiont(CURRENT_POINT &nextPT )
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
//    EnterCriticalSection(&m_csCommunicationSync);  

	if( m_CurrentPiontInfo.lineIndex  ==lineIndex && m_CurrentPiontInfo.pintIndex )
	{
		m_CurrentPiontInfo.status = status;
	}
	    /** �뿪�ٽ��� */   
//    LeaveCriticalSection(&m_csCommunicationSync);
}

///��������������
void CGlobalAirLine::SetExposurePoint(int lineIndex, int pointIndex )
{
	/** �ٽ������� */   
    EnterCriticalSection(&m_csCommunicationSync);  

	if( lineIndex > 0 && pointIndex > 0 )
	{
		int newint = JoinInt(lineIndex, pointIndex );
		m_pointList.push_back(newint);

		///��Ӻ������ع�����
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

	 /** �뿪�ٽ��� */   
    LeaveCriticalSection(&m_csCommunicationSync);
}

///��ȡ�ع���Ƿ��ع�״̬
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
		///��ȡ�������ع�����
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
