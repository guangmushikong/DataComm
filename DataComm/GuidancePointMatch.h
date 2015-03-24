#pragma once
#include "datadefine.h"
#include "include\ogrsf_frmts.h"
#include <map>
#include <vector>
#include "GuidancePointMatch.h"

class GuidancePointMatch
{
	class GaussProjection
	{
	public:
		GaussProjection(OGRPoint center);
		~GaussProjection(void);
		void getGussCoord(OGRPoint& p);
	private:
		OGRPoint centerPoint;
	};
public:
	GuidancePointMatch(void);
	~GuidancePointMatch(void);
	
	void readGuidancePoint(std::string filepath);
	bool getMatchedGP(GuidancePoint& tgrGP, GPRMC plane);
	bool getGaussCoord(OGRPoint& p);
	void setDistanceCriteria(double _criteria);
	void setHeadingCriteria(double _criteria);

	void testGetCenterPoint();
	void testGetDistance();

private:
	void registerGhtFile(std::string filePath);
	double getDistance(OGRPoint p1, OGRPoint p2);
	int getLineIndexFromHeader(const std::string& header);
	int getPointIndexFromHeader(const std::string& header);
	void releaseBuffer();
	void getCenterPoint();
	int getOptimalGP(std::vector<GuidancePoint*>& vtrGPs, GPRMC plane);
	bool bTopologyMatched(std::string currheader, std::string preheader);
private:
	double dDistanceCriteria;
	double dHeadingCriteria;
	int nCurrentAirLine;
	//std::string strPointIndex; // A1/A2/B1/B2/number
	//std::string header;
	OGRPoint centerPoint; // used to set Gauss Projection param
	GuidancePoint currentGP;
	std::map<int, std::vector<GuidancePoint*>* > mapGPs;
	GaussProjection* pGaussProj;
};

