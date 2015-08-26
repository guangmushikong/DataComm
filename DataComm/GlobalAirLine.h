#pragma once

#include "COMMSTRUCT.h"
#include "stdafx.h"

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

	///ǰ����
	static CURRENT_POINT m_NextPiontInfo;

	///��������б�
	static list<int>  m_pointList;

	/** ͬ������,�ٽ������� */   
    CRITICAL_SECTION   m_csCommunicationSync; 

public:
	///��ȡ���µ��������Ϣ
	void GetCurrentPiont(CURRENT_POINT &curPT);

	///�������µ��������Ϣ
	void SetCurrentPiont(const CURRENT_POINT curPT);

    ///��ȡǰ������Ϣ
	void GetNextPiont(CURRENT_POINT &nextPT);

	///����ǰ������Ϣ
	void SetNextPiont(const CURRENT_POINT nextPT);

	///�������������״̬
	void SetPointStatus(int lineIndex, int pintIndex, bool status);

	///��������������
    void SetExposurePoint(int lineIndex, int pointIndex );
		///��ȡ�ع���Ƿ��ع�״̬
	bool GetExposurePointStatus(int lineIndex, int pointIndex);

	int  JoinInt(int front,int back);
};

