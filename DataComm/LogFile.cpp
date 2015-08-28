#include "StdAfx.h"
#include "LogFile.h"
#include <time.h>
#include <iostream>
#include "SystemParam.h"

CLogFile * CLogFile::m_LogFile = NULL;
CLogFile::CLogFile(void):m_bOpen(FALSE)
{
	//��ʼ��ͬ������
	InitializeCriticalSection(&m_LogSec);
	
	Task_Info taskInfo;
	CSystemParam::GetTaskInfo(taskInfo);
	//����output
	CreateDirectoryA(taskInfo.output_path.c_str(), NULL);
	//����taskĿ¼
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
//���ļ�
BOOL CLogFile::Open()
{
	try
	{
		if( (m_pFile = fopen(m_FileName.c_str(),"a")) == NULL )
		{
			std::cout << "����Ŀ¼����ʧ�ܣ���ȷ��Output·���Ƿ���ȷ��" ; 
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


//д���ļ�
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

		///ÿ�м�ʱ��
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


	//�ر��ļ�
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
