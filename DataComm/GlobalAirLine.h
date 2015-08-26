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

	///当前拍摄点信息
	static CURRENT_POINT m_CurrentPiontInfo;

	///前方点
	static CURRENT_POINT m_NextPiontInfo;

	///已拍摄点列表
	static list<int>  m_pointList;

	/** 同步互斥,临界区保护 */   
    CRITICAL_SECTION   m_csCommunicationSync; 

public:
	///获取最新的拍摄点信息
	void GetCurrentPiont(CURRENT_POINT &curPT);

	///设置最新的拍摄点信息
	void SetCurrentPiont(const CURRENT_POINT curPT);

    ///获取前方点信息
	void GetNextPiont(CURRENT_POINT &nextPT);

	///设置前方点信息
	void SetNextPiont(const CURRENT_POINT nextPT);

	///设置拍摄点拍摄状态
	void SetPointStatus(int lineIndex, int pintIndex, bool status);

	///添加已拍摄拍摄点
    void SetExposurePoint(int lineIndex, int pointIndex );
		///获取曝光点是否曝光状态
	bool GetExposurePointStatus(int lineIndex, int pointIndex);

	int  JoinInt(int front,int back);
};

