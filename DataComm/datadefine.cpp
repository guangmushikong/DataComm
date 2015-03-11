#include "stdafx.h"
#include "datadefine.h"

#ifdef _MS_MFC
MyPointF::MyPointF(MyPointF& _p)
{
	dx = _p.getX();
	dy = _p.getY();
	dz = _p.getZ();
}

MyPointF::MyPointF(double _dx, double _dy, double _dz):dx(_dx),dy(_dy),dz(_dz)
{}

MyPointF& MyPointF::operator =(MyPointF& _p)
{
	dx = _p.getX();
	dy = _p.getY();
	dz = _p.getZ();
    return(*this);
}

void MyPointF::getCoord(double &_x, double &_y, double &_z)
{
  _x = dx;
  _y = dy;
  _z = dz;
}

void MyPointF::setCoord(double _x, double _y, double _z)
{
  dx = _x;
  dy = _y;
  dz = _z;
}
#endif
