// DataComm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "SerialPort.h" 
#include "ComReceiver.h"
#include "Exposure.h"
#include "SystemParam.h"
#include "MatchAirLinePoint.h"
#include "SqliteManger.h"
#include "LogFile.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	///��ʼ��ϵͳ��������ȡ����
	CSystemParam  param;
	param.IniSysParam();

	COMM_PARAM commParam;
	param.GetGpsCommParam(commParam);
	CLogFile *pFile;
	pFile->GetInstance()->WriteLog("ϵͳ������", 10);
    CSerialPort serialPort;  
 
	if (!serialPort.InitPort(commParam.port, commParam.baud))  
    {  
        std::cout << "initPort fail !" << std::endl;  
    }  
    else 
    {  
        std::cout << "initPort success !" << std::endl;  
    }  
 
    if (!serialPort.OpenListenThread())  
    {  
        std::cout << "OpenListenThread fail !" << std::endl;  
    }  
    else 
    {  
        std::cout << "OpenListenThread success !" << std::endl;  
    }  

	CSqliteManger::GetInstance()->InitDatabase();
   
	///�ع⴦�����
	CExposure exposure;
	exposure.OpenDataProcessThread();

	///����ƥ�����
	CMatchAirLinePoint match;
	match.OpenMatchProcessThread();

	serialPort.AddObserver(&exposure);
	serialPort.AddObserver(&match);

	int temp;
	std::cin >> temp; 

	return 0;
}

