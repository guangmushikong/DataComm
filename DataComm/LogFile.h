/*
* @date:		 2015-3-23
* @version       1.0	
* @history 	 
*	<修改者> <修改日期> <版本> <修改内容简述>
*	孙跃锋	  2011-07-18 2.0	修改该文件
*/

#include <iostream>  
#include "stdafx.h"

#define  MAX_BUFSIZE		8196
#define  MAX_LENGTH	    	80
#define  PFIX_LENGTH	    15  //文件名中日期部分之前的部分
#define  CAT_LENGTH		    3

//定义日志路径
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
	///单例
	static CLogFile * GetInstance();
	/*!
	* @brief:		获得日志文件名称函数
	* @description:	获取日志文件名称
	* @param:		无
	* @return:		string：日志文件名
	*/
//	string GetFileName();	
	/*!
	* @brief:		设置日志文件名称头
	* @description:	将输入参数作为日志名称的头，以区别信息处理产生的各类日志文件
	* @param:		szPrefix：输入参数：日志名称的头
	* @return:		N/A
	*/
//	void SetLogFilePrefix(char *szPrefix);	
	/*!
	* @brief:		记录日志
	* @description:	将程序运行期间的错误或重要信息写入日志文件中
	* @param:		szMsg：输入参数：要记录的日志信息
	* @param:		nLen：输入参数：长度
	* @return:		N/A
	*/
	void WriteLog(const char* szMsg, int nLen );
private:
	/*!
	* @brief:		将缓冲区写入日志
	* @description:	将缓冲区写入日志
	* @param:		无
	* @return:		N/A
	*/
//	void    LogFileFlush();
	/*!
	* @brief:		打开日志文件
	* @description:	以写方式打开文件
	* @return:		true：成功；false：失败
		*/
	BOOL	Open();
	
	/*!
	* @brief:		关闭日志文件
	* @description:	关闭日志文件
	* @param:		无
	* @return:		N/A
	*/
	void    Close();
	/*!
	* @brief:		写文件
	* @description:	将信息写入日志文件中
	* @param:		szContent：输入参数：要记录的信息
	* @param:		nStrlen：输入参数：长度
	* @return:		N/A
	*/
	void	WriteToLog(const char* szContent,int nStrlen=0);
public:
	/*!
	* @brief:		获取时间
	* @description:	获取写日志的当前时间
	* @param:		N/A
	* @param:		N/A
	* @return:		当前时间的字符串
	*/
    void GetTime(char * strTM);

private:	
	//文件变量
	FILE*	m_pFile;
	
	//存放文件名			
	string		m_FileName;

	string      m_FilePath;
	
	//判断文件是否打开的变量
	BOOL		m_bOpen;
	
	//获得临界区变量用以同步日志文件的读写					
	CRITICAL_SECTION  m_LogSec;	

	static CLogFile *m_LogFile;
};

