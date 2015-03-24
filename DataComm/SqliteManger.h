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
	///��ʼ�����ݿ�
	bool InitDatabase();
	///����λ������
	bool InsertPosition(GPRMC posInfo);
	///�����ع���Ϣ
	bool InsertExposure(GPRMC posInfo,CURRENT_POINT ExpInfo, double delay, string status);
};

