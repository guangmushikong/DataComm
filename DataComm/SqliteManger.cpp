#include "StdAfx.h"
#include "SqliteManger.h"
#include "LogFile.h"

CSqliteManger * CSqliteManger::m_SqliteMange = NULL;

CSqliteManger::CSqliteManger(void)
{
	m_pDB = NULL;
}


CSqliteManger * CSqliteManger::GetInstance()
{
	if( m_SqliteMange == NULL )
	{
		m_SqliteMange = new CSqliteManger();
	}

	return m_SqliteMange;
}

bool CSqliteManger::InitDatabase()
{
	int nRes = sqlite3_open(DATABASENAME, &m_pDB);
	if( nRes !=SQLITE_OK )
	{
		cout<<"Open database fail"<<sqlite3_errmsg(m_pDB);
		sqlite3_close(m_pDB);
		return false;
	}

	char * cErrMsg;
	string strSqlPos = "CREATE TABLE position( \
					   ID        INTEGER PRIMARY KEY AUTOINCREMENT,\
					   DataTime  TEXT,\
					   latitude  NUMERIC(10),\
					   longitude NUMERIC(10), \
					   hight     NUMERIC(10),\
					   velocity  NUMERIC(10), \
					   azimuth   NUMERIC(10) );";

	nRes = sqlite3_exec(m_pDB, strSqlPos.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"create new table fail"<<cErrMsg<<endl;
	}
	else
	{
		cout<<"create new table Success"<<endl;
	}

	string strSqlExp = "CREATE TABLE Exposure( \
					   ID        INTEGER PRIMARY KEY AUTOINCREMENT,\
					   DataTime  TEXT,\
					   latitude  NUMERIC(10),\
					   longitude NUMERIC(10), \
					   hight     NUMERIC(10),\
					   velocity  NUMERIC(10), \
					   azimuth   NUMERIC(10),\
					   line_lat  NUMERIC(10),\
					   line_lon  NUMERIC(10), \
					   line_hgt  NUMERIC(10),\
					   line_num  INTEGER,\
					   point_num INTEGER, \
					   delay     NUMERIC(10),\
					   status    TEXT);";

	nRes = sqlite3_exec(m_pDB, strSqlExp.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"create new table fail"<<cErrMsg<<endl;
	}
	else
	{
		cout<<"create new table Success"<<endl;
	}

	return 0;
}

bool CSqliteManger::InsertPosition(GPRMC posInfo)
{
	string strInsPos = "INSERT INTO position( \
					        DataTime, longitude, latitude, hight, velocity, azimuth )  \
						VALUES ( '";
	char strTM[80];
	CLogFile *pFile;
	pFile->GetInstance()->GetTime(strTM);
	strInsPos += strTM;
	strInsPos += "'";
	char cPos[80];
	sprintf(cPos,",%f, %f, %f, %f ,%f );", posInfo.pos.lon,posInfo.pos.lat,posInfo.pos.high,
						                posInfo.vel, posInfo.az);
	strInsPos += cPos;

	char * cErrMsg;
	int nRes = sqlite3_exec(m_pDB, strInsPos.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"add new position fail"<<cErrMsg<<endl;
		return false;
	}
	else
	{
		cout<<"add new position Success"<<endl;
		return true;
	}
}

bool CSqliteManger::InsertExposure(GPRMC posInfo,CURRENT_POINT ExpInfo, double delay, string status)
{
	string strInsExp = "INSERT INTO Exposure( \
					        DataTime, longitude, latitude, hight,    velocity,  azimuth, \
							line_lon, line_lat,  line_hgt, line_num, point_num, delay , status)  \
						VALUES ( ";
	strInsExp += "'2001-03-34 13:21:30'";
	char cPos[180];
	sprintf(cPos,",%f, %f, %f, %f ,%f, %f, %f, %f,%d, %d, %f, \'%s\' );", 
		         posInfo.pos.lon,posInfo.pos.lat,posInfo.pos.high, posInfo.vel, posInfo.az,
				 ExpInfo.position.lon,ExpInfo.position.lat,ExpInfo.position.high,
				 ExpInfo.lineIndex,ExpInfo.pintIndex,delay, status.c_str());
	strInsExp += cPos;

	char * cErrMsg;
	int nRes = sqlite3_exec(m_pDB, strInsExp.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"add new position fail"<<cErrMsg<<endl;
		return false;
	}
	else
	{
		cout<<"add new position Success"<<endl;
		return true;
	}
}