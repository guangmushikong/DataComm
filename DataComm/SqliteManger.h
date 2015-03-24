#pragma once

#include "sqlite3.h"
#include <iostream>
#include "COMMSTRUCT.h"

using namespace std;

#define  DATABASENAME  ".\\DataInfo.db"


class CSqliteManger
{
public:
	CSqliteManger(void);
public:
	static CSqliteManger * GetInstance();

	static void Release();

public:
	sqlite3 *m_pDB;

	static CSqliteManger * m_SqliteMange;
public:
	///初始化数据库
	bool InitDatabase();
	///插入位置数据
	bool InsertPosition(GPRMC posInfo);
	///插入曝光信息
	bool InsertExposure(GPRMC posInfo,CURRENT_POINT ExpInfo, double delay, string status);
};

