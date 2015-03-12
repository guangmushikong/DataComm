#include "StdAfx.h"
#include "SystemParam.h"
#include <sstream>
#include <fstream>
#include "datadefine.h"

EXPOSURE_PARAM    CSystemParam::m_exposureParam;
std::map<std::string, GuidancePointPtr> CSystemParam::m_mapAirLinePTInfo;
std::string CSystemParam::head("01-0A1");

CSystemParam::CSystemParam(void)
{
}


CSystemParam::~CSystemParam(void)
{
}

void CSystemParam::IniSysParam()
{
	registerGhtFile(CONFIG_AIRLINE_PATH_NAME, m_mapAirLinePTInfo);

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
                                   /*std::vector<GuidancePoint*>& vtrGPs*/
								   std::map<std::string, GuidancePointPtr> mapGPs)
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
		iss.str(line);

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

GuidancePoint CSystemParam::getMatchingPointFromGPs(const COORDINATE& p)
{
	//GuidancePoint point(GuidancePoint::AirPort, p);
	//int index = point.getMinDistanceIndex(m_vtrAirLinePTInfo);
	//GuidancePoint* pRlt = m_vtrAirLinePTInfo[index];
	//return GuidancePoint(*pRlt);
	std::map<string, GuidancePointPtr>::iterator it = m_mapAirLinePTInfo.find(head);
	GuidancePointPtr rlt = 0;
	if (it != m_mapAirLinePTInfo.end())
	{
		rlt = it->second;
		// edit the head to prepare for the next find
		if (std::string::npos != head.find("A1")) // A1 ==> A2
		{
			head.replace(head.find("A1"), 2, "A2");
		}
		else if (std::string::npos != head.find("A2")) // A2 ==> normal
		{
			head.replace(head.find("A2"), 2, "01");
		}
		else if (std::string::npos != head.find("B2")) // B2 ==> B1
		{
			head.replace(head.find("B2"), 2, "B1");
		}
		else if (std::string::npos != head.find("B1")) // B1 ==> A1
		{
			std::string lineIndex = head.substr(0, 2);
			int nLineIndex = atoi(lineIndex.c_str());
			nLineIndex += 1;
			char buffer[7];
			sprintf(buffer, "%02d-%s", nLineIndex, "0A1");
			head = buffer;
			// it's possible that the map don't consist the new head
		}
		else // normal ==> normal or normal ==> B2
		{
			std::string pointIndex = head.substr(3, 3);
			int nPointIndex = atoi(pointIndex.c_str());
			nPointIndex += 1;
			char buffer[7];
			sprintf(buffer, "%s-%03d", head.substr(0, 2), nPointIndex);
			head = buffer;
			if (m_mapAirLinePTInfo.end() == m_mapAirLinePTInfo.find(head))
			{
				sprintf(buffer, "%s-%s", head.substr(0, 2), "0B2");
				head = buffer;
				if (m_mapAirLinePTInfo.end() == m_mapAirLinePTInfo.find(head))
				{
					// there must be error, so report error
				}
			}
		}
	}
	else
	{
		// maybe report error
	}
	return GuidancePoint(*rlt);
}