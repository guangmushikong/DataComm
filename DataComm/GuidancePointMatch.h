#pragma once
#include "datadefine.h"
//#include "include\ogrsf_frmts.h"
#include <map>
#include <vector>
#include "GuidancePointMatch.h"

class IGuidancePointMatch
{
public:
	IGuidancePointMatch(){}
	~IGuidancePointMatch(){}
	virtual bool getMatchedGP(GuidancePoint& tgrGP, GPRMC plane) = 0;
	virtual void readGuidancePoint(std::string filepath) = 0;
	virtual void setDistanceCriteria(double) = 0;
	virtual void setHeadingCriteria(double) = 0;
	virtual void setExposureRate(double) = 0;
};

class decorateGPMatch : public IGuidancePointMatch
{
public:
	decorateGPMatch(IGuidancePointMatch* p);
	~decorateGPMatch();
	bool getMatchedGP(GuidancePoint& tgrGP, GPRMC plane);
	void readGuidancePoint(std::string filepath);
	void setDistanceCriteria(double);
	void setHeadingCriteria(double);
	void setExposureRate(double);
protected:
	IGuidancePointMatch* pGPMatch;
	std::string logfile; 
};

class GuidancePointMatch : public IGuidancePointMatch
{
public:
	GuidancePointMatch(void);
	~GuidancePointMatch(void);
	
	void readGuidancePoint(std::string filepath);
	bool getMatchedGP(GuidancePoint& tgrGP, GPRMC plane);
	void setDistanceCriteria(double _criteria);
	void setHeadingCriteria(double _criteria);
	void setExposureRate(double _criteria);

private:
	int  getMatchedLine(const GPRMC& plane);
	int  getMatchedLine(const GPRMC& plane, int flag);
	int  binaryMatchedLine(const GPRMC& plane, int beginLineIdx, int endLineIdx);
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
	double getLineDistance(const std::vector<GuidancePoint*>& vtrGPs, const GPRMC& plane);
	double getLinePlaneAngle(double _angle1, double _angle2);
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

