#include "StdAfx.h"
#include "GaussProjection.h"


//GaussProjection::GaussProjection(OGRPoint center)
//{
//	centerPoint.setX(center.getX());
//	centerPoint.setY(center.getY());
//	centerPoint.setZ(center.getZ());
//}
//
//
//GaussProjection::~GaussProjection(void)
//{
//}
//
//void GaussProjection::getGussCoord(OGRPoint& p)
//{
//	OGRSpatialReference spaRef;
//	spaRef.SetProjCS("Local Guass-Krugger on WGS84");
//	spaRef.SetWellKnownGeogCS("WGS84");
//    spaRef.SetTM( 0, 
//		          centerPoint.getX(), 
//				  0.9996,
//                  500000.0, 
//				  (centerPoint.getY() >= 0.0) ? 0.0 : 10000000.0 );
//	OGRSpatialReference* pSpaRef = spaRef.CloneGeogCS();
//	OGRCoordinateTransformation* pTrans = OGRCreateCoordinateTransformation(pSpaRef, &spaRef);
//	p.transform(pTrans);
//}
