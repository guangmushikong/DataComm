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

double GuidancePoint::getDistance(const COORDINATE& _p)
{
	double lat_diff = point.lat - _p.lat;
	double lon_diff = point.lon - _p.lon;
	return (lat_diff*lat_diff + lon_diff*lon_diff);
}

int GuidancePoint::getMinDistanceIndex(const std::vector<GuidancePoint*>& vtrGPs)
{
	if(vtrGPs.size() < 1)
		return 0;
	double dMinDistance; // default value
	int nIndex;
	for(int i=0; i<vtrGPs.size(); ++i)
	{
		COORDINATE _p = vtrGPs[i]->point;
		double dDistance = getDistance(_p);
		// initialize the dMinDistance
		if(0 == i)
		{
			dMinDistance = dDistance;
			nIndex = i;
			continue;
		}

		if(dDistance < dMinDistance)
		{
			dMinDistance = dDistance;
			nIndex = i;
		}
	}
	return nIndex;
}
#endif
