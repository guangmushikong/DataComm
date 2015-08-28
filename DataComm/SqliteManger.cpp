#include "StdAfx.h"
#include "SqliteManger.h"
#include "LogFile.h"
#include "SystemParam.h"
#include "GlobalAirLine.h"
#include "COMMSTRUCT.h"

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
	m_strDBName = DBName;

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
		cout<<"create position table fail"<<cErrMsg<<endl;
	}
	else
	{
		cout<<"create position table Success"<<endl;
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
		cout<<"create Exposure table fail"<<cErrMsg<<endl;
	}
	else
	{
		cout<<"create Exposure table Success"<<endl;
	}

	string strSqlSys = "CREATE TABLE SystemStatus( \
					   ID             INTEGER PRIMARY KEY AUTOINCREMENT,\
					   DataTime       TEXT,\
					   StatusString   TEXT,\
					   StatusType     NUMERIC(2));";

	nRes = sqlite3_exec(m_pDB, strSqlSys.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"create SystemStatus table fail"<<cErrMsg<<endl;
	}
	else
	{
		cout<<"create SystemStatus table Success"<<endl;
	}

	string strSqlFrontPt = "CREATE TABLE FrontPointInfo( \
					   ID             INTEGER PRIMARY KEY AUTOINCREMENT,\
					   DataTime       TEXT,\
					   line_index     INTEGER,\
					   point_sequence INTEGER,\
					   latitude       NUMERIC(10),\
					   longitude      NUMERIC(10),\
					   hight          NUMERIC(10) );";

	nRes = sqlite3_exec(m_pDB, strSqlFrontPt.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"create FrontPointInfo table fail"<<cErrMsg<<endl;
	}
	else
	{
		cout<<"create FrontPointInfo table Success"<<endl;
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

bool CSqliteManger::InsertSystemStatus(System_Status status)
{
	string strInsPos = "INSERT INTO SystemStatus( \
					        DataTime, StatusString, StatusType )  \
						VALUES ( '";
	char strTM[80];
	CLogFile *pFile;
	pFile->GetInstance()->GetTime(strTM);
	strInsPos += strTM;
	strInsPos += "'";
	char cPos[80];
	switch(status)
	{
	case SYS_Normal:
		sprintf(cPos,",'%s', %d );", "曝光模式", status);
		break;
	case SYS_ShootMiss:
		sprintf(cPos,",'%s', %d );", "补拍模式",status);
		break;
	case SYS_Spiral:
		sprintf(cPos,",'%s', %d );", "盘旋模式",status);
		break;
	case SYS_BackAirport:
		sprintf(cPos,",'%s', %d );", "返航模式",status);
		break;
	default:
		return true;
	}

	strInsPos += cPos;

	char * cErrMsg;
	int nRes = sqlite3_exec(m_pDB, strInsPos.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"add new System_Status fail"<<cErrMsg<<endl;
		return false;
	}
	else
	{
		cout<<"add new System_Status Success"<<endl;
		return true;
	}
}

bool CSqliteManger::InsertFrontPointInfo(CURRENT_POINT ExpInfo)
{
	string strInsPos = "INSERT INTO FrontPointInfo( \
					        DataTime, line_index, point_sequence, longitude, latitude, hight )  \
						VALUES ( '";
	char strTM[80];
	CLogFile *pFile;
	pFile->GetInstance()->GetTime(strTM);
	strInsPos += strTM;
	strInsPos += "'";
	char cPos[80];
	sprintf(cPos,",%d, %d, %f, %f, %f );", ExpInfo.lineIndex, ExpInfo.sequence, 
		                                   ExpInfo.position.lat, ExpInfo.position.lon, ExpInfo.position.high);
	strInsPos += cPos;

	char * cErrMsg;
	int nRes = sqlite3_exec(m_pDB, strInsPos.c_str(), 0, 0, &cErrMsg);
	if(nRes != SQLITE_OK)
	{
		cout<<"add new Front Point Fail"<<cErrMsg<<endl;
		return false;
	}
	else
	{
		cout<<"add new Front Point Success"<<endl;
		return true;
	}
}

bool CSqliteManger::InitSysFromDB()
{
	if (m_sqliteWrapper.Open(m_strDBName)) 
	{  
		std::cout << "SQLiteWrapper.db created or opened" << std::endl;  
	}  
	else 
	{  
		std::cout << "culdn't open SQLiteWrapper.db" << std::endl;  
	}  

	SQLiteStatement* stmt = m_sqliteWrapper.Statement("select distinct line_Index, point_Index from Exposure Order by ID;");  
	if(stmt == NULL)
	{
		return false;
	}

	int lineIndex, pointIndex;
	while (stmt->NextRow()) 
	{  
		string strline = stmt->ValueString(0);
		lineIndex = atoi(strline.c_str());

		string strpoint = stmt->ValueString(1);
		pointIndex = atoi(strpoint.c_str());

		CGlobalAirLine::GetInstance()->SetExposurePoint(lineIndex, pointIndex);
	}

	SQLiteStatement* stmtf = m_sqliteWrapper.Statement("SELECT line_index, point_sequence from FrontPointInfo Order BY ID DESC limit 1;");  
	if(stmtf == NULL)
	{
		return false;
	}

	int lineNum = 0, sequence = 0;
	while (stmtf->NextRow()) 
	{  
		string strline = stmtf->ValueString(0);
		lineNum = atoi(strline.c_str());

		string strpoint = stmtf->ValueString(1);
		sequence = atoi(strpoint.c_str());

		CURRENT_POINT nextPT;
		nextPT.lineIndex = lineNum;
		nextPT.sequence  = sequence;
		CGlobalAirLine::GetInstance()->SetNextPiont(nextPT);
	}

	SQLiteStatement* stmts = m_sqliteWrapper.Statement("SELECT StatusType from SystemStatus Order BY ID DESC limit 1;");  
	if(stmts == NULL)
	{
		return false;
	}

	while (stmts->NextRow()) 
	{  
		string strpeType = stmts->ValueString(0);
		int type = atoi(strpeType.c_str());
		CSystemParam::SetSystemStatus((System_Status)type);
	}

	return true;
}