#include "StdAfx.h"
#include "SqliteManger.h"
#include "LogFile.h"
#include "SystemParam.h"

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
	Task_Info taskInfo;
	CSystemParam::GetTaskInfo(taskInfo);
	string DBName = taskInfo.output_path  + taskInfo.task_name + DATABASENAME;

	int nRes = sqlite3_open(DBName.c_str(), &m_pDB);
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
					   gpsTime   NUMERIC(10),\
					   latitude  NUMERIC(10),\
					   longitude NUMERIC(10),\
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
					   ID          INTEGER PRIMARY KEY AUTOINCREMENT,\
					   DataTime    TEXT,\
					   gpsTime     NUMERIC(10),\
					   latitude    NUMERIC(10),\
					   longitude   NUMERIC(10),\
					   hight       NUMERIC(10),\
					   velocity    NUMERIC(10),\
					   azimuth     NUMERIC(10),\
					   line_lat    NUMERIC(10),\
					   line_lon    NUMERIC(10),\
					   line_hgt    NUMERIC(10),\
					   line_index  INTEGER,\
					   point_index INTEGER,\
					   H_Distance  NUMERIC(10),\
					   V_Distance  NUMERIC(10));";

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
					        DataTime, gpsTime, longitude, latitude, hight, velocity, azimuth )  \
						VALUES ( '";
	char strTM[80];
	CLogFile *pFile;
	pFile->GetInstance()->GetTime(strTM);
	strInsPos += strTM;
	strInsPos += "'";
	char cPos[80];
	sprintf(cPos,",%f, %f, %f, %f, %f ,%f );", posInfo.time, posInfo.pos.lon,posInfo.pos.lat,posInfo.pos.high,
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

bool CSqliteManger::InsertExposure(GPRMC posInfo,CURRENT_POINT ExpInfo)
{
	string strInsExp = "INSERT INTO Exposure( \
					        DataTime, gpsTime, longitude, latitude, hight, velocity, azimuth, \
							line_lon, line_lat, line_hgt, line_index, point_index, H_Distance, V_Distance )  \
						VALUES ( '";
	char strTM[80];
	CLogFile *pFile;
	pFile->GetInstance()->GetTime(strTM);
	strInsExp += strTM;
	strInsExp += "'";
	char cPos[180];
	sprintf(cPos,",%f, %f, %f, %f, %f ,%f, %f, %f, %f, %d, %d, %f, %f);", 
		         posInfo.time,     posInfo.pos.lon, posInfo.pos.lat,
				 posInfo.pos.high, posInfo.vel,     posInfo.az,
				 ExpInfo.position.lon, ExpInfo.position.lat, ExpInfo.position.high,
				 ExpInfo.lineIndex,    ExpInfo.pintIndex, 
				 ExpInfo.h_distance,   ExpInfo.v_distance);

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