#include "StdAfx.h"
#include "LogFile.h"
#include <time.h>
#include <iostream>
#include "SystemParam.h"

CLogFile * CLogFile::m_LogFile = NULL;
CLogFile::CLogFile(void):m_bOpen(FALSE)
{
	//初始化同步变量
	InitializeCriticalSection(&m_LogSec);
	
	Task_Info taskInfo;
	CSystemParam::GetTaskInfo(taskInfo);
	//创建output
	CreateDirectoryA(taskInfo.output_path.c_str(), NULL);
	//创建task目录
	m_FilePath = taskInfo.output_path + taskInfo.task_name;
	CreateDirectoryA(m_FilePath.c_str(), NULL);

    m_FileName = m_FilePath + LOGNAME;
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
		if( (m_pFile = fopen(m_FileName.c_str(),"a")) == NULL )
		{
			std::cout << "工作目录创建失败，请确认Output路径是否正确：" ; 
			m_bOpen = false;
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

	strftime(strTM,80,"%Y-%m-%d %H:%M:%S ",timeInfo);
}
