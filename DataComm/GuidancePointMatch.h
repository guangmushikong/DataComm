#pragma once
#include "datadefine.h"
//#include "include\ogrsf_frmts.h"
#include <map>
#include <vector>
#include "GuidancePointMatch.h"

class GuidancePointMatch
{
	//class GaussProjection
	//{
	//public:
	//	GaussProjection(OGRPoint center);
	//	~GaussProjection(void);
	//	void getGussCoord(OGRPoint& p);
	//private:
	//	OGRPoint centerPoint;
	//};
	class ExposureRate
	{
	public:
		int totalPointNum;
		int exposurePointNum;
	};
public:
	GuidancePointMatch(void);
	~GuidancePointMatch(void);
	
	void readGuidancePoint(std::string filepath);
	bool getMatchedGP(GuidancePoint& tgrGP, GPRMC plane);
	//bool getGaussCoord(OGRPoint& p);
	void setDistanceCriteria(double _criteria);
	void setHeadingCriteria(double _criteria);
	void setExposureRate(double _criteria);

	void testGetCenterPoint();
	void testGetDistance();

private:
	int  getMatchedLine(const GPRMC& plane);
	void registerGhtFile(std::string filePath);
	//double getDistance(OGRPoint p1, OGRPoint p2);
	double getDistance(COORDINATE p1, COORDINATE p2);
	double getRelaDistance(COORDINATE p1, COORDINATE p2, COORDINATE p);
	int getLineIndexFromHeader(const std::string& header);
	int getPointIndexFromHeader(const std::string& header);
	void releaseBuffer();
	//void getCenterPoint();
	int getOptimalGP(const std::vector<GuidancePoint*>& vtrGPs, const GPRMC& plane);
	bool bTopologyMatched(std::string currheader, std::string preheader);
	void GetProjectionPt(COORDINATE ptA, double az_B, COORDINATE ptB,  COORDINATE &out_pt );
	void initExposureRate();
	double getLineAngle(const std::vector<GuidancePoint*>& vtrGPs, const GPRMC& plane);
	//double getLineExposureRate(const std::vector<GuidancePoint*>& vtrGPs);
private:
	double dDistanceCriteria;
	double dHeadingCriteria;
	double dExposureCriteria;
	int nCurrentAirLine;
	//std::string strPointIndex; // A1/A2/B1/B2/number
	//std::string header;
	//OGRPoint centerPoint; // used to set Gauss Projection param
	GuidancePoint currentGP;
	std::map<int, std::vector<GuidancePoint*>* > mapGPs;
	std::map<int, ExposureRate> mapExposureLine;
	//GaussProjection* pGaussProj;
};

