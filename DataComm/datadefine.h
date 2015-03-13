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

typedef struct GuidancePoint
{
    enum GuidancePointType
    {
        AirPort,
        A1Type,
        A2Type,
        B1Type,
        B2Type,
        Normal
    };
#ifdef _MS_MFC
	GuidancePoint()
	{
		point.high = .0;
		point.lat  = .0;
		point.lon  = .0;
		nLineNum   = 0;
		nPointNum  = 0;
	}
    GuidancePoint(GuidancePointType t, /*MyPointF*/COORDINATE p, int lVle=0, int pVle=0):
        type(t),point(p), nLineNum(lVle), nPointNum(pVle){};
	GuidancePoint(const GuidancePoint& GP)
	{
		this->point.high = GP.point.high;
		this->point.lat  = GP.point.lat;
		this->point.lon  = GP.point.lon;
		this->type = GP.type;
		this->nLineNum = GP.nLineNum;
		this->nPointNum = GP.nPointNum;
	}
    int getMinDistanceIndex(const std::vector<GuidancePoint*>& vtrGPs);
	COORDINATE point;
#else
    GuidancePoint(GuidancePointType t, QPointF p, int lVle=0, int pVle=0):
        type(t),point(p), nLineNum(lVle), nPointNum(pVle){};
    QPointF point; // the coordinate
#endif
    int nLineNum; // belong to which line
    int nPointNum; // the sequence number
    GuidancePointType type; // airport or A1 or A2 or B1 or B2

	bool distanceMatchFlag;
	bool headingMatchFlag;
	bool posingMatchFlag;
	bool topologyMatchFlag;

protected:
	double getDistance(const COORDINATE& _p);

} *GuidancePointPtr;

#endif // DATADEFINE_H
