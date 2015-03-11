#ifndef DATADEFINE_H
#define DATADEFINE_H

//#include <QPointF>
#include "stdafx.h"

#ifdef _MS_MFC
class MyPointF
{
public:
  MyPointF(double _dx, double _dy, double _dz=.0);
  MyPointF(MyPointF& _p);
  MyPointF& operator=(MyPointF& _p);
  void getCoord(double& _x, double& _y, double& _z);
  void setCoord(double _x, double _y, double _z);
  double getX(){ return dx; }
  double getY(){ return dy; }
  double getZ(){ return dz; }
private:
  double dx;
  double dy;
  double dz;
};
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
    GuidancePoint(GuidancePointType t, MyPointF p, int lVle=0, int pVle=0):
        type(t),point(p), nLineNum(lVle), nPointNum(pVle){};
    MyPointF point; // the coordinate
#else
    GuidancePoint(GuidancePointType t, QPointF p, int lVle=0, int pVle=0):
        type(t),point(p), nLineNum(lVle), nPointNum(pVle){};
    QPointF point; // the coordinate
#endif
    int nLineNum; // belong to which line
    int nPointNum; // the sequence number
    GuidancePointType type; // airport or A1 or A2 or B1 or B2

} *GuidancePointPtr;

#endif // DATADEFINE_H
