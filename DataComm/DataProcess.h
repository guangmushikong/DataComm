#pragma once

#include "COMMSTRUCT.h"

class CDataProcess
{
public:
	CDataProcess(void);
	~CDataProcess(void);
public:
	void UnPackGPGGA( const string &data, GPGGA *pMsg );

	void UnPackGPVTG( const string &data, GPVTG *pMsg );

	///最简定位信息
	void UnPackGPRMC( const string &data, GPRMC *pMsg );
};

