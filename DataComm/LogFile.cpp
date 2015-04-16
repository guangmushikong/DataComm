#include "StdAfx.h"
#include "LogFile.h"
#include <time.h>
#include <iostream>


CLogFile * CLogFile::m_LogFile = NULL;
CLogFile::CLogFile(void):m_bOpen(FALSE)
{
	//初始化同步变量
	InitializeCriticalSection(&m_LogSec);

    m_FileName = LOGPATH;
	m_FileName += LOGNAME;
}

CLogFile::~CLogFile(void)
{
//	Close();
//	DeleteCriticalSection(&m_LogSec);
}

CLogFile * CLogFile::GetInstance()
{
	if( m_LogFile == NULL )
	{
		m_LogFile = new CLogFile();
	}

	return m_LogFile;
}
//打开文件
BOOL CLogFile::Open()
{
	try
	{
		//创建文件目录
        CreateDirectoryA(LOGPATH, NULL);
	
		if( (m_pFile = fopen(m_FileName.c_str(),"a")) == NULL )
		{
			std::cout << "日志文件创建失败" ; 
		}
		m_bOpen = true;
	}
	catch(...)
	{
		m_bOpen=FALSE;
	}
	return m_bOpen;
}


//写入文件
void CLogFile::WriteToLog(const char* szContent,int nStrlen)
{
	char szMsg[MAX_BUFSIZE];
	try
	{
		ZeroMemory(szMsg,sizeof(szMsg));
		if(!(m_bOpen))
			return;

		if ( nStrlen<=0 || nStrlen > MAX_BUFSIZE)
		{
			return;
		}

		///每行加时间
		char strTM[80];
		GetTime(strTM);
		string strMsg;
		strMsg = strTM;
		strMsg += szContent;
		strMsg += "\r\n";
		
		fwrite(strMsg.c_str(), 1, strMsg.length(), m_pFile);
        fclose(m_pFile);
	}
	catch(...)
	{
		return;
	}
}

void CLogFile::WriteLog(const char* szMsg, int nLen)
{	
	EnterCriticalSection(&m_LogSec);

	if(m_bOpen)
	{
	    Close();
	}

	if(Open())
	{
		WriteToLog(szMsg, nLen);
	}
	
	LeaveCriticalSection(&m_LogSec);
}


	//关闭文件
void CLogFile::Close()
{
	if(TRUE == m_bOpen)
	{
		fclose(m_pFile);
	}
	m_bOpen = FALSE;
}
void CLogFile::GetTime(char * strTM)
{
	time_t rawtime;
	struct tm * timeInfo;
	time(&rawtime);
	timeInfo = localtime(&rawtime);

	strftime(strTM,80,"%Y-%m-%d %I:%M:%S: ",timeInfo);
}
