#include "StdAfx.h"
#include "SystemParam.h"
#include <sstream>
#include <fstream>
#include "datadefine.h"

EXPOSURE_PARAM    CSystemParam::m_exposureParam;
std::map<std::string, GuidancePointPtr> CSystemParam::m_mapAirLinePTInfo;
std::string CSystemParam::head("01-0A1");
BoundingBox* CSystemParam::pBoundingBox=0;
COMM_PARAM CSystemParam::m_cameraCommParam;
COMM_PARAM CSystemParam::m_gpsCommParam;

CSystemParam::CSystemParam(void)
{
}


CSystemParam::~CSystemParam(void)
{
}

void CSystemParam::IniSysParam()
{
	///获取航迹点信息
	registerGhtFile(CONFIG_AIRLINE_PATH_NAME, m_mapAirLinePTInfo);

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

	///获取相机串口参数
	m_cameraCommParam.port =  GetPrivateProfileIntA("CAMERA", "COMID", 0, CONFIGFILENAME);
	m_cameraCommParam.baud =  GetPrivateProfileIntA("CAMERA", "BAUD",  0, CONFIGFILENAME);

	///获取GPS接收机串口参数
	m_gpsCommParam.port =  GetPrivateProfileIntA("GPS", "COMID", 0, CONFIGFILENAME);
	m_gpsCommParam.baud =  GetPrivateProfileIntA("GPS", "BAUD",  0, CONFIGFILENAME);
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

void CSystemParam::registerGhtFile(std::string filePath,
                                   /*std::vector<GuidancePoint*>& vtrGPs*/
								   std::map<std::string, GuidancePointPtr>& mapGPs)
{
    if(filePath.empty())
        return;
#ifndef _MS_MFC
    QVector<QPointF> vtr;
#endif
    std::istringstream iss;
    char cline[255];
    std::string head;
    std::string pointType;
    double dCoordX, dCoordY, dCoordZ;
    int lineNum, PointNum, type;
    GuidancePoint* pGP = 0;
	// the 2nd line of the file present the bounding box
	// the bounding box should be stored 
	unsigned int unFileLine = 0;

    std::ifstream infile(filePath);
    if(infile.fail() || infile.bad())
    {
        return;
    }

    while(infile.getline(cline, 255))
    {
        std::string line(cline);
	    iss.clear();
		iss.str(line);

		if (2 == (++unFileLine))
		{
			pBoundingBox = new BoundingBox;
			double dMinX, dMinY, dMaxX, dMaxY;
			iss >> dMinX >> dMaxX >> dMinY >> dMaxY;
			pBoundingBox->minPoint.lat = dMinX;
			pBoundingBox->minPoint.lon = dMinY;
			pBoundingBox->maxPoint.lat = dMaxX;
			pBoundingBox->maxPoint.lon = dMaxY;
		}

        if(std::string::npos != line.find("AIRPORT"))
        {
            //iss.str(line);
            //iss >> head >> dCoordX >> dCoordY >> type;
            //pGP = new GuidancePoint(GuidancePoint::AirPort, QPointF(dCoordY, dCoordX));
            continue;
        }
        else if(std::string::npos != line.find("-"))
        {
			iss >> head >> dCoordX >> dCoordY >> dCoordZ >> type;
			iss.clear();
			iss.str(head);
			if (std::string::npos != line.find("A") || 
				std::string::npos != line.find("B"))
			{
				iss >> lineNum >> pointType;
			}
			else
				iss >> lineNum >> PointNum;
            
            if(std::string::npos != line.find("A1"))
            {
				COORDINATE point; point.lon = dCoordY; point.lat = dCoordX;
                pGP = new GuidancePoint(GuidancePoint::A1Type, 
#ifndef _MS_MFC
										QPointF(dCoordY, dCoordX),
#else
										point,
#endif
										lineNum);

            }
            else if(std::string::npos != line.find("A2"))
            {
				COORDINATE point; point.lon = dCoordY; point.lat = dCoordX;
                pGP = new GuidancePoint(GuidancePoint::A2Type,
#ifndef _MS_MFC
										QPointF(dCoordY, dCoordX),
#else
										point,
#endif
										lineNum);

            }
            else if(std::string::npos != line.find("B1"))
            {
				COORDINATE point; point.lon = dCoordY; point.lat = dCoordX;
                pGP = new GuidancePoint(GuidancePoint::B1Type,
#ifndef _MS_MFC
										QPointF(dCoordY, dCoordX),
#else
										point,
#endif
										lineNum);
            }
            else if(std::string::npos != line.find("B2"))
            {
				COORDINATE point; point.lon = dCoordY; point.lat = dCoordX;
                pGP = new GuidancePoint(GuidancePoint::B2Type,
#ifndef _MS_MFC
										QPointF(dCoordY, dCoordX),
#else
										point,
#endif
										lineNum);
            }
            else
            {
				COORDINATE point; point.lon = dCoordY; point.lat = dCoordX;
                pGP = new GuidancePoint(GuidancePoint::Normal,
#ifndef _MS_MFC
										QPointF(dCoordY, dCoordX),
#else
										point,
#endif
										lineNum, PointNum);
            }
        }
        else
            continue;
#ifndef _MS_MFC
        vtr.push_back(QPointF(dCoordY, dCoordX));
        drawObject::registerGuidancePoint(pGP);
#else
		mapGPs.insert(std::make_pair(head, pGP));
#endif
    }
    infile.close();

#ifndef _MS_MFC
	void* pData = new QPolygonF(vtr);
	registerData((void*)pData, shapeData::polyline);
#endif
}

void CSystemParam::GetAirLinePTList( std::vector<GuidancePoint*>& vtrGPs )
{
	//vtrGPs = m_vtrAirLinePTInfo;
}

void CSystemParam::getNextGPHead(const std::string& _head, std::string& nextHead)
{
	nextHead = _head;

	if (std::string::npos != nextHead.find("A1")) // A1 ==> A2
	{
		nextHead.replace(nextHead.find("A1"), 2, "A2");
	}
	else if (std::string::npos != nextHead.find("A2")) // A2 ==> normal
	{
		nextHead.replace(nextHead.find("A2"), 2, "01");
	}
	else if (std::string::npos != nextHead.find("B2")) // B2 ==> B1
	{
		nextHead.replace(nextHead.find("B2"), 2, "B1");
	}
	else if (std::string::npos != nextHead.find("B1")) // B1 ==> A1
	{
		std::string lineIndex = nextHead.substr(0, 2);
		int nLineIndex = atoi(lineIndex.c_str());
		nLineIndex += 1;
		char buffer[7];
		sprintf(buffer, "%02d-%s", nLineIndex, "0A1");
		nextHead = buffer;
		// it's possible that the map don't consist the new head
	}
	else // normal ==> normal or normal ==> B2
	{
		std::string pointIndex = nextHead.substr(3, 3);
		int nPointIndex = atoi(pointIndex.c_str());
		nPointIndex += 1;
		char buffer[7];
		sprintf(buffer, "%s-%03d", nextHead.substr(0, 2), nPointIndex);
		nextHead = buffer;
		if (m_mapAirLinePTInfo.end() == m_mapAirLinePTInfo.find(nextHead))
		{
			sprintf(buffer, "%s-%s", nextHead.substr(0, 2), "0B2");
			nextHead = buffer;
			if (m_mapAirLinePTInfo.end() == m_mapAirLinePTInfo.find(nextHead))
			{
				// there must be error, so report error
			}
		}
	}
}

GuidancePoint CSystemParam::getMatchingPointFromGPs(const COORDINATE& p)
{
	GuidancePointPtr rlt = new GuidancePoint();
	std::map<string, GuidancePointPtr>::iterator it = m_mapAirLinePTInfo.find(head);
	if (it != m_mapAirLinePTInfo.end())
	{
		rlt = it->second;
		// edit the head to prepare for the next find
		getNextGPHead(head, head);
	}
	else
	{
		// maybe report error
	}
	return GuidancePoint(*rlt);
}