/*
* @date:		 2015-3-23
* @version       1.0	
* @history 	 
*	<�޸���> <�޸�����> <�汾> <�޸����ݼ���>
*	��Ծ��	  2011-07-18 2.0	�޸ĸ��ļ�
*/

#include <iostream>  
#include "stdafx.h"

#define  MAX_BUFSIZE		8196
#define  MAX_LENGTH	    	80
#define  PFIX_LENGTH	    15  //�ļ��������ڲ���֮ǰ�Ĳ���
#define  CAT_LENGTH		    3

//������־·��
#define  LOGPATH    ".\\Output\\" 
#define  LOGNAME    "\\DataComm.log"

#pragma once
class CLogFile
{
public:
	CLogFile();
	CLogFile(char* szPrefix);	
	virtual ~CLogFile();
public:
	///����
	static CLogFile * GetInstance();
	/*!
	* @brief:		�����־�ļ����ƺ���
	* @description:	��ȡ��־�ļ�����
	* @param:		��
	* @return:		string����־�ļ���
	*/
//	string GetFileName();	
	/*!
	* @brief:		������־�ļ�����ͷ
	* @description:	�����������Ϊ��־���Ƶ�ͷ����������Ϣ��������ĸ�����־�ļ�
	* @param:		szPrefix�������������־���Ƶ�ͷ
	* @return:		N/A
	*/
//	void SetLogFilePrefix(char *szPrefix);	
	/*!
	* @brief:		��¼��־
	* @description:	�����������ڼ�Ĵ������Ҫ��Ϣд����־�ļ���
	* @param:		szMsg�����������Ҫ��¼����־��Ϣ
	* @param:		nLen���������������
	* @return:		N/A
	*/
	void WriteLog(const char* szMsg, int nLen );
private:
	/*!
	* @brief:		��������д����־
	* @description:	��������д����־
	* @param:		��
	* @return:		N/A
	*/
//	void    LogFileFlush();
	/*!
	* @brief:		����־�ļ�
	* @description:	��д��ʽ���ļ�
	* @return:		true���ɹ���false��ʧ��
		*/
	BOOL	Open();
	
	/*!
	* @brief:		�ر���־�ļ�
	* @description:	�ر���־�ļ�
	* @param:		��
	* @return:		N/A
	*/
	void    Close();
	/*!
	* @brief:		д�ļ�
	* @description:	����Ϣд����־�ļ���
	* @param:		szContent�����������Ҫ��¼����Ϣ
	* @param:		nStrlen���������������
	* @return:		N/A
	*/
	void	WriteToLog(const char* szContent,int nStrlen=0);
public:
	/*!
	* @brief:		��ȡʱ��
	* @description:	��ȡд��־�ĵ�ǰʱ��
	* @param:		N/A
	* @param:		N/A
	* @return:		��ǰʱ����ַ���
	*/
    void GetTime(char * strTM);

private:	
	//�ļ�����
	FILE*	m_pFile;
	
	//����ļ���			
	string		m_FileName;

	string      m_FilePath;
	
	//�ж��ļ��Ƿ�򿪵ı���
	BOOL		m_bOpen;
	
	//����ٽ�����������ͬ����־�ļ��Ķ�д					
	CRITICAL_SECTION  m_LogSec;	

	static CLogFile *m_LogFile;
};

