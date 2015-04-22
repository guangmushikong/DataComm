#include "stdafx.h"
#include "datadefine.h"

#ifdef _MS_MFC
//MyPointF::MyPointF(MyPointF& _p)
//{
//	dx = _p.getX();
//	dy = _p.getY();
//	dz = _p.getZ();
//}
//
//MyPointF::MyPointF(double _dx, double _dy, double _dz):dx(_dx),dy(_dy),dz(_dz)
//{}
//
//MyPointF& MyPointF::operator =(MyPointF& _p)
//{
//	dx = _p.getX();
//	dy = _p.getY();
//	dz = _p.getZ();
//    return(*this);
//}
//
//void MyPointF::getCoord(double &_x, double &_y, double &_z)
//{
//  _x = dx;
//  _y = dy;
//  _z = dz;
//}
//
//void MyPointF::setCoord(double _x, double _y, double _z)
//{
//  dx = _x;
//  dy = _y;
//  dz = _z;
//}
GuidancePoint::GuidancePoint()
{
	point.high = .0;
	point.lat  = .0;
	point.lon  = .0;
	nLineIndex   = 0;
	nPointIndex  = 0;
	pointHeader = "";
}

GuidancePoint::~GuidancePoint()
{
}
GuidancePoint::GuidancePoint(GuidancePointType t, COORDINATE p, int lIndex, int pIndex):
type(t),point(p), nLineIndex(lIndex), nPointIndex(pIndex)
{		
}

GuidancePoint::GuidancePoint(const GuidancePoint& GP)
{
	this->point.high = GP.point.high;
	this->point.lat  = GP.point.lat;
	this->point.lon  = GP.point.lon;
	this->type = GP.type;
	this->nLineIndex = GP.nLineIndex;
	this->nPointIndex = GP.nPointIndex;
	this->pointHeader = GP.pointHeader;
}

#endif

GuidancePointStatus::GuidancePointStatus()
{
	bAirLineMatched = false;
	bDistanceMatched = false;
	bHeadingMatched = false;
	bPosingMatched = false;
	bTopologyMatched = false;
	bExposure = false;
}

GuidancePointStatus::~GuidancePointStatus()
{
}

void GuidancePointStatus::resetStatus()
{
	bAirLineMatched = false;
	bDistanceMatched = false;
	bHeadingMatched = false;
	bPosingMatched = false;
	bTopologyMatched = false;
	bExposure = false;
}
