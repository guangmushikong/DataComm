#include "StdAfx.h"
#include "SystemParam.h"
#include <sstream>
#include <fstream>
#include "datadefine.h"

EXPOSURE_PARAM    CSystemParam::m_exposureParam;
std::vector<GuidancePoint*> CSystemParam::m_vtrAirLinePTInfo;
CSystemParam::CSystemParam(void)
{
}


CSystemParam::~CSystemParam(void)
{
}

void CSystemParam::IniSysParam()
{
	registerGhtFile(CONFIG_AIRLINE_PATH_NAME, m_vtrAirLinePTInfo);

//	char buf[20];
//	LPCWSTR my = "EXPOSURE" ;
//    GetPrivateProfileString("EXPOSURE", "DELAY", "", (char*)buf, 20, CONFIGFILENAME);
//	m_exposureParam.delay = atof(buf);

//	GetPrivateProfileString("EXPOSURE", "DISTAN", "", (char*)buf, 20, CONFIGFILENAME);
//	m_exposureParam.distan = atof(buf);

//    GetPrivateProfileString("EXPOSURE", "ANGLE", "", (char*)buf, 20, CONFIGFILENAME);
//	m_exposureParam.angle = atof(buf);
}

void CSystemParam::GetExposurParam(EXPOSURE_PARAM &expParam)
{
	expParam = m_exposureParam;
}

void CSystemParam::registerGhtFile(std::string filePath,
                                   std::vector<GuidancePoint*>& vtrGPs)
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

    std::ifstream infile(filePath);
    if(infile.fail() || infile.bad())
    {
        return;
    }

    while(infile.getline(cline, 255))
    {
        std::string line(cline);
	    iss.clear();

        if(std::string::npos != line.find("AIRPORT"))
        {
            //iss.str(line);
            //iss >> head >> dCoordX >> dCoordY >> type;
            //pGP = new GuidancePoint(GuidancePoint::AirPort, QPointF(dCoordY, dCoordX));
            continue;

        }
        else if(std::string::npos != line.find("-"))
        {
			line.replace(line.find("-"), 1, " ");
			iss.str(line);
            
            if(std::string::npos != line.find("A1"))
            {
				iss >> lineNum >> pointType >> dCoordX >> dCoordY >> dCoordZ >> type;
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
				iss >> lineNum >> pointType >> dCoordX >> dCoordY >> dCoordZ >> type;
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
				iss >> lineNum >> pointType >> dCoordX >> dCoordY >> dCoordZ >> type;
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
				iss >> lineNum >> pointType >> dCoordX >> dCoordY >> dCoordZ >> type;
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
				iss >> lineNum >> PointNum >> dCoordX >> dCoordY >> dCoordZ >> type;
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
		vtrGPs.push_back(pGP);
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
	vtrGPs = m_vtrAirLinePTInfo;
}

GuidancePoint CSystemParam::getMatchingPointFromGPs(const COORDINATE& p)
{
	GuidancePoint point(GuidancePoint::AirPort, p);
	int index = point.getMinDistanceIndex(m_vtrAirLinePTInfo);
	GuidancePoint* pRlt = m_vtrAirLinePTInfo[index];
	return GuidancePoint(*pRlt);
}