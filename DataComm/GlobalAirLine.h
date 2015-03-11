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

	///当前拍摄点信息
	static CURRENT_POINT m_CurrentPiontInfo;

	/** 同步互斥,临界区保护 */   
    CRITICAL_SECTION   m_csCommunicationSync; 
public:
	///获取最新的拍摄点信息
	void GetCurrentPiont(CURRENT_POINT &curPT);

	///设置最新的拍摄点信息
	void SetCurrentPiont(const CURRENT_POINT curPT);

	///设置拍摄点拍摄状态
	void SetPointStatus(int lineIndex, int pintIndex, bool status);
};

