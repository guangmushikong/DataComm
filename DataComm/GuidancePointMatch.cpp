#include "StdAfx.h"
#include "GuidancePointMatch.h"
#include "Exposure.h"
#include "GEGeoCaculate.h"
#include <sstream>
#include <fstream>


void GuidancePointMatch::testGetCenterPoint()
{
	std::string filepath = "E:/GitHub/test_data/kkk2.ght";
	readGuidancePoint(filepath);
}

void GuidancePointMatch::testGetDistance()
{
	OGRPoint p1(116.58080, 39.51298);
	OGRPoint p2(116.58029, 39.51217);
	double dDistance = getDistance(p1, p2);
}

GuidancePointMatch::GuidancePointMatch(void)
{
	pGaussProj = 0;
	dDistanceCriteria = 10.0;
	dHeadingCriteria = 20.0;
	//header = "01-0A1";
	nCurrentAirLine = 1;
}


GuidancePointMatch::~GuidancePointMatch(void)
{
}

void GuidancePointMatch::setDistanceCriteria(double _criteria)
{
	dDistanceCriteria = _criteria;
}

void GuidancePointMatch::setHeadingCriteria(double _criteria)
{
	dHeadingCriteria = _criteria;
}

void GuidancePointMatch::registerGhtFile(std::string filePath)
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
	std::vector<GuidancePoint*>* pVtrGPs = new std::vector<GuidancePoint*>; // used to store GPs on one airline
    GuidancePoint* pGP = 0;
	int nPreLnIdx = 1; // used to check if GPs on one airline finish
	int nLineIdx;

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
			std::string _head = head;
			nLineIdx = getLineIndexFromHeader(head);
			_head.replace(head.find("-"), 1, " ");
			iss.clear();
			iss.str(_head);
			if (std::string::npos != line.find("A") || 
				std::string::npos != line.find("B"))
			{
				iss >> lineNum >> pointType;
			}
			else
			{
				iss >> lineNum >> PointNum;
			}

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
		pGP->pointHeader = head;
		if(nLineIdx == nPreLnIdx)
		{
			pVtrGPs->push_back(pGP);
		}
		else
		{
			mapGPs.insert(std::make_pair(nPreLnIdx, pVtrGPs));
			pVtrGPs = new std::vector<GuidancePoint*>;
			pVtrGPs->push_back(pGP);
			nPreLnIdx = nLineIdx;
		}
#endif
    }
    infile.close();

#ifndef _MS_MFC
	void* pData = new QPolygonF(vtr);
	registerData((void*)pData, shapeData::polyline);
#endif
}

double GuidancePointMatch::getDistance(OGRPoint p1, OGRPoint p2)
{
	//pGaussProj->getGussCoord(p1);
	//pGaussProj->getGussCoord(p2);
	double dP1GaussX, dP1GaussY, dP2GaussX, dP2GaussY;
	CGEGeoCaculate cal;
	cal.BL2XY_Gauss(p1.getX(), p1.getY(), dP1GaussX, dP1GaussY);
	cal.BL2XY_Gauss(p2.getX(), p2.getY(), dP2GaussX, dP2GaussY);
	double dx = dP1GaussX - dP2GaussX;
	double dy = dP2GaussX - dP2GaussY;
	return sqrtf(dx*dx + dy*dy);
}

bool GuidancePointMatch::getGaussCoord(OGRPoint& p)
{
	if (0 == pGaussProj)
	{
		return false;
	}

	pGaussProj->getGussCoord(p);
	return true;
}

int GuidancePointMatch::getLineIndexFromHeader(const std::string& header)
{
	std::string strLnIdx = header.substr(0, 2);
	return atoi(strLnIdx.c_str());
}

int GuidancePointMatch::getPointIndexFromHeader(const std::string& header)
{
	std::string strPnIdx = header.substr(3);
	if (std::string::npos == strPnIdx.find("A") && 
		std::string::npos == strPnIdx.find("B"))
	{
		return atoi(strPnIdx.c_str());
	}
	else
	{
		return 0;
	}
}

void GuidancePointMatch::releaseBuffer()
{
	std::map<int, std::vector<GuidancePoint*>* >::iterator it;
	for (it = mapGPs.begin(); it != mapGPs.end(); ++it)
	{
		std::vector<GuidancePoint*>* pVtrGPs = it->second;
		for (std::vector<GuidancePoint*>::iterator itVtr = pVtrGPs->begin();
			 itVtr != pVtrGPs->end(); ++itVtr)
		{
			delete (*itVtr);
		}
	}
	if (pGaussProj)
	{
		delete pGaussProj;
	}
}

int GuidancePointMatch::getOptimalGP(std::vector<GuidancePoint*>& vtrGPs, GPRMC plane)
{
	int index;
	double dDistance;
	for (int it = 0; it < vtrGPs.size(); ++it)
	{
		const GuidancePoint* pGP = vtrGPs.at(it);
		double dX = pGP->point.lat - plane.pos.lat;
		double dY = pGP->point.lon - plane.pos.lon;
		double dTmpDistance = dX*dX + dY*dY;
		if (it == 0)
		{
			dDistance = dTmpDistance;
			index = 0;
		}
		else if (dDistance > dTmpDistance)
		{
			dDistance = dTmpDistance;
			index = it;
		}
	}
	return index;
}

bool GuidancePointMatch::getMatchedGP(GuidancePoint& tgrGP, GPRMC plane)
{
	OGRPoint _plane(plane.pos.lon, plane.pos.lat);
	//int nCurrentAirLine = getLineIndexFromHeader(header);
	
	COORDINATE pStart;
	COORDINATE pEnd;
	double dAirLineHeading = .0;
	int nIdxFlag = 0;
	
	std::map<int, std::vector<GuidancePoint*>* >::iterator it = mapGPs.find(nCurrentAirLine);
	if(mapGPs.end() != it)
	{
		std::vector<GuidancePoint*> vtrRltPoint;
		std::vector<GuidancePoint*>* pVtrGPs = it->second;
		// distance criteria
		for (std::vector<GuidancePoint*>::iterator it = pVtrGPs->begin(); 
			it != pVtrGPs->end(); ++it)
		{
			GuidancePoint* pGP = *it;
			pGP->resetStatus();
			OGRPoint gp(pGP->point.lon, pGP->point.lat);
			double dTmpDistance = getDistance(_plane, gp);
			if(dTmpDistance < dDistanceCriteria)
			{
				pGP->setAirLineMatchedStatus(true);
				pGP->setDistanceMatchedStatus(true);
				vtrRltPoint.push_back(pGP);
			}

			// used to calculate heading angle
			if (1 == nIdxFlag)
			{
				pEnd.lat = pGP->point.lat;
				pEnd.lon = pGP->point.lon;
				pEnd.high = pGP->point.high;
			}
			else if (0 == nIdxFlag)
			{
				pStart.lat = pGP->point.lat;
				pStart.lon = pGP->point.lon;
				pStart.high = pGP->point.high;
			}
			++nIdxFlag;
		}
		// heading criteria
		dAirLineHeading = CExposure::GetAngleFrom2Points(pEnd, pStart);
		bool bHeadingMatched = (abs(dAirLineHeading-plane.az) < dHeadingCriteria);
		for (std::vector<GuidancePoint*>::iterator it= vtrRltPoint.begin(); 
			 it !=  vtrRltPoint.end(); ++it)
		{
			GuidancePoint* pGP = *it;
			pGP->setHeadingMatchedStatus(bHeadingMatched);
		}

		// posing criteria

		// topology criteria -- don't implement for now
		int GPIdx = getOptimalGP(vtrRltPoint, plane);
		GuidancePoint* pOptimalGP = vtrRltPoint.at(GPIdx);
		tgrGP = *pOptimalGP;
		if (std::string::npos != tgrGP.pointHeader.find("B1"))
		{
			++nCurrentAirLine;
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool GuidancePointMatch::bTopologyMatched(std::string currheader, std::string preheader)
{
	int currIdx = getPointIndexFromHeader(currheader);
	int preIdx  = getPointIndexFromHeader(preheader);

	return true;
}

void GuidancePointMatch::getCenterPoint()
{
	centerPoint.setX(.0);
	centerPoint.setY(.0);

	std::map<int, std::vector<GuidancePoint*>* >::iterator itMap;
	std::vector<GuidancePoint*>::iterator itVtr;
	int nAirLineSize = 0;
	for (itMap=mapGPs.begin(); itMap!=mapGPs.end(); ++itMap)
	{
		++nAirLineSize;
		int nGPOneLineSize = 0;
		COORDINATE lineCenter;
		lineCenter.high = .0;
		lineCenter.lat  = .0;
		lineCenter.lon  = .0;
		std::vector<GuidancePoint*>* pVtrGPs = itMap->second;
		for (itVtr=pVtrGPs->begin(); itVtr!=pVtrGPs->end(); ++itVtr)
		{
			++nGPOneLineSize;
			GuidancePoint* pGP = *itVtr;
			lineCenter.lat += pGP->point.lat;
			lineCenter.lon += pGP->point.lon;
		}
		lineCenter.lat = lineCenter.lat/nGPOneLineSize;
		lineCenter.lon = lineCenter.lon/nGPOneLineSize;

		centerPoint.setX(centerPoint.getX() + lineCenter.lon);
		centerPoint.setY(centerPoint.getY() + lineCenter.lat);
	}
	centerPoint.setX(centerPoint.getX()/nAirLineSize);
	centerPoint.setY(centerPoint.getY()/nAirLineSize);
}

void GuidancePointMatch::readGuidancePoint(std::string filepath)
{
	registerGhtFile(filepath);
	getCenterPoint();
	if (0==pGaussProj)
	{
		pGaussProj = new GaussProjection(centerPoint);
	}
}

GuidancePointMatch::GaussProjection::GaussProjection(OGRPoint center)
{
	centerPoint.setX(center.getX());
	centerPoint.setY(center.getY());
	centerPoint.setZ(center.getZ());
}


GuidancePointMatch::GaussProjection::~GaussProjection(void)
{
}

void GuidancePointMatch::GaussProjection::getGussCoord(OGRPoint& p)
{
	OGRSpatialReference spaRef;
	spaRef.SetProjCS("Local Guass-Krugger on WGS84");
	spaRef.SetWellKnownGeogCS("WGS84");
	spaRef.SetTM( 0, 
		centerPoint.getX(), 
		0.9996,
		500000.0, 
		(centerPoint.getY() >= 0.0) ? 0.0 : 10000000.0 );
	OGRSpatialReference* pSpaRef = spaRef.CloneGeogCS();
	OGRCoordinateTransformation* pTrans = OGRCreateCoordinateTransformation(pSpaRef, &spaRef);
	p.transform(pTrans);
}

