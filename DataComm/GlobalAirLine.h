#pragma once

#include "COMMSTRUCT.h"

class CGlobalAirLine
{
public:
	static CGlobalAirLine * GetInstance();

	static void Release();
private:
	CGlobalAirLine(void);

	static CGlobalAirLine * m_globalAirLine;

	///��ǰ�������Ϣ
	static CURRENT_POINT m_CurrentPiontInfo;

	/** ͬ������,�ٽ������� */   
    CRITICAL_SECTION   m_csCommunicationSync; 
public:
	///��ȡ���µ��������Ϣ
	void GetCurrentPiont(CURRENT_POINT &curPT);

	///�������µ��������Ϣ
	void SetCurrentPiont(const CURRENT_POINT curPT);

	///�������������״̬
	void SetPointStatus(int lineIndex, int pintIndex, bool status);
};

