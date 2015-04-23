#include "StdAfx.h"
#include "SystemParam.h"
#include <sstream>
#include <fstream>
#include "datadefine.h"

EXPOSURE_PARAM    CSystemParam::m_exposureParam;
COMM_PARAM CSystemParam::m_cameraCommParam;
COMM_PARAM CSystemParam::m_gpsCommParam;
decorateGPMatch* CSystemParam::GP_Match = 0;
UDP_PARAM CSystemParam::m_udpParam;

CSystemParam::CSystemParam(void)
{
}


CSystemParam::~CSystemParam(void)
{
}

void CSystemParam::IniSysParam()
{
	///获取曝光容差参数
	char buf[20];
    int num = 0;
    num =  GetPrivateProfileStringA("EXPOSURE", "DELAY", "", (char*)buf, 20, CONFIGFILENAME);
	if( num > 0 )
	{
		m_exposureParam.delay = atof( buf );
	}
	memset(buf, 0, 20);
	num =  GetPrivateProfileStringA("EXPOSURE", "DISTAN ", "", (char*)buf, 20, CONFIGFILENAME);
	if( num > 0 )
	{
		m_exposureParam.distan= atof( buf );
	}
	memset(buf, 0, 20);
	num =  GetPrivateProfileStringA("EXPOSURE", "ANGLE", "", (char*)buf, 20, CONFIGFILENAME);
	if( num > 0 )
	{
		m_exposureParam.angle= atof( buf );
	}
    m_exposureParam.trigger =  GetPrivateProfileIntA("EXPOSURE", "TRIGGER",  0, CONFIGFILENAME);
	m_exposureParam.frequency =  GetPrivateProfileIntA("EXPOSURE", "FREQUENCY",  0, CONFIGFILENAME);
	m_exposureParam.rate =  GetPrivateProfileIntA("EXPOSURE", "RATE",  0, CONFIGFILENAME);
	m_exposureParam.rate/= 100;

	///获取相机串口参数
	m_cameraCommParam.port =  GetPrivateProfileIntA("CAMERA", "COMID", 0, CONFIGFILENAME);
	m_cameraCommParam.baud =  GetPrivateProfileIntA("CAMERA", "BAUD",  0, CONFIGFILENAME);

	///获取GPS接收机串口参数
	m_gpsCommParam.port =  GetPrivateProfileIntA("GPS", "COMID", 0, CONFIGFILENAME);
	m_gpsCommParam.baud =  GetPrivateProfileIntA("GPS", "BAUD",  0, CONFIGFILENAME);

	///获取UDP参数
	m_udpParam.port = GetPrivateProfileIntA("UDP", "PORT", 0, CONFIGFILENAME);

	///获取航迹点信息
	GP_Match = new decorateGPMatch(new GuidancePointMatch);
	GP_Match->readGuidancePoint(CONFIG_AIRLINE_PATH_NAME);
	GP_Match->setDistanceCriteria(m_exposureParam.distan);
	GP_Match->setHeadingCriteria(m_exposureParam.angle);
	GP_Match->setExposureRate( m_exposureParam.rate);
}

void CSystemParam::GetExposurParam(EXPOSURE_PARAM &param)
{
	param = m_exposureParam;
}

void CSystemParam::GetCameraCommParam(COMM_PARAM &param)
{
	param = m_cameraCommParam;
}

void CSystemParam::GetGpsCommParam(COMM_PARAM &param)
{
	param = m_gpsCommParam;
}

void CSystemParam::GetUDPParam(UDP_PARAM &param)
{
	param = m_udpParam;
}
//void CSystemParam::getNextGPHead(const std::string& _head, std::string& nextHead)
//{
//	nextHead = _head;
//
//	if (std::string::npos != nextHead.find("A1")) // A1 ==> A2
//	{
//		nextHead.replace(nextHead.find("A1"), 2, "A2");
//	}
//	else if (std::string::npos != nextHead.find("A2")) // A2 ==> normal
//	{
//		nextHead.replace(nextHead.find("A2"), 2, "01");
//	}
//	else if (std::string::npos != nextHead.find("B2")) // B2 ==> B1
//	{
//		nextHead.replace(nextHead.find("B2"), 2, "B1");
//	}
//	else if (std::string::npos != nextHead.find("B1")) // B1 ==> A1
//	{
//		std::string lineIndex = nextHead.substr(0, 2);
//		int nLineIndex = atoi(lineIndex.c_str());
//		nLineIndex += 1;
//		char buffer[7];
//		sprintf(buffer, "%02d-%s", nLineIndex, "0A1");
//		nextHead = buffer;
//		// it's possible that the map don't consist the new head
//	}
//	else // normal ==> normal or normal ==> B2
//	{
//		std::string pointIndex = nextHead.substr(3, 3);
//		int nPointIndex = atoi(pointIndex.c_str());
//		nPointIndex += 1;
//		char buffer[7];
//		sprintf(buffer, "%s-%03d", nextHead.substr(0, 2), nPointIndex);
//		nextHead = buffer;
//		if (m_mapAirLinePTInfo.end() == m_mapAirLinePTInfo.find(nextHead))
//		{
//			sprintf(buffer, "%s-%s", nextHead.substr(0, 2), "0B2");
//			nextHead = buffer;
//			if (m_mapAirLinePTInfo.end() == m_mapAirLinePTInfo.find(nextHead))
//			{
//				// there must be error, so report error
//			}
//		}
//	}
//}
