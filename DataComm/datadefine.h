#ifndef DATADEFINE_H
#define DATADEFINE_H

//#include <QPointF>
#include "stdafx.h"
#include "COMMSTRUCT.h"

#ifdef _MS_MFC
//class MyPointF
//{
//public:
//  MyPointF(double _dx, double _dy, double _dz=.0);
//  MyPointF(MyPointF& _p);
//  MyPointF& operator=(MyPointF& _p);
//  void getCoord(double& _x, double& _y, double& _z);
//  void setCoord(double _x, double _y, double _z);
//  double getX(){ return dx; }
//  double getY(){ return dy; }
//  double getZ(){ return dz; }
//private:
//  double dx;
//  double dy;
//  double dz;
//};
#endif

class GuidancePointStatus
{
	friend class GuidancePoint;
public:
	GuidancePointStatus();
	~GuidancePointStatus();
	void resetStatus();
private:
	bool bAirLineMatched;
	bool bDistanceMatched;
	bool bHeadingMatched;
	bool bPosingMatched;
	bool bTopologyMatched;
	bool bExposure;
};

class GuidancePoint
{
public:

#ifdef _MS_MFC
	GuidancePoint();
	GuidancePoint(GuidancePointType t, COORDINATE p, int lIndex=0, int pIndex=0);
	//GuidancePoint(std::string header, COORDINATE p);
	GuidancePoint(const GuidancePoint& GP);
	~GuidancePoint();
	void resetStatus(){ GP_Status.resetStatus(); }
	void setAirLineMatchedStatus(bool status){ GP_Status.bAirLineMatched = status; }
	bool getAirLineMatchedStatus(){ return GP_Status.bAirLineMatched; }
	void setDistanceMatchedStatus(bool status){ GP_Status.bDistanceMatched = status; }
	bool getDistanceMatchedStatus(){ return GP_Status.bDistanceMatched; }
	void setHeadingMatchedStatus(bool status){ GP_Status.bHeadingMatched = status; }
	bool getHeadingMatchedStatus(){ return GP_Status.bHeadingMatched; }
	void setPosingMatchedStatus(bool status){ GP_Status.bPosingMatched = status; }
	bool getPosingMatchedStatus(){ return GP_Status.bPosingMatched; }
	void setTopologyMatchedStatus(bool status){ GP_Status.bTopologyMatched = status; }
	bool getTopologyMatchedStatus(){ return GP_Status.bTopologyMatched; }
	void setExposureStatus(bool status){ GP_Status.bExposure = status; }
	bool getExposureStatus(){ return GP_Status.bExposure; }
	COORDINATE point;
#else
	GuidancePoint(GuidancePointType t, QPointF p, int lVle=0, int pVle=0):
		type(t),point(p), nLineNum(lVle), nPointNum(pVle){};
		QPointF point; // the coordinate
#endif
		std::string pointHeader;
		int nLineIndex; // belong to which line
		int nPointIndex; // the sequence number
		GuidancePointType type;
		GuidancePointStatus GP_Status;
		int nSequNum;
};

class ExposureRate
{
public:
	int totalPointNum;
	int exposurePointNum;
};

#endif // DATADEFINE_H
