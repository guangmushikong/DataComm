#pragma once

#include "sqlite3.h"
#include <iostream>
#include "COMMSTRUCT.h"
#include "SystemParam.h"
#include "SQLiteWrapper.h"

using namespace std;

#define  DATABASENAME  "\\DataInfo.db"


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

	string		m_FileName;

	string      m_FilePath;

	SQLiteWrapper m_sqliteWrapper;

	string m_strDBName;
public:
	///��ʼ�����ݿ�
	bool InitDatabase();
	///����λ������
	bool InsertPosition(GPRMC posInfo);
	///�����ع���Ϣ
	bool InsertExposure(GPRMC posInfo,CURRENT_POINT ExpInfo);

	bool InsertFrontPointInfo(CURRENT_POINT ExpInfo);
	///����ϵͳ״̬
	bool InsertSystemStatus(System_Status status);

	bool InitSysFromDB();
};

