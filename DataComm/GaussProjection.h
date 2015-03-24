#pragma once

#include "include\ogrsf_frmts.h"

class GaussProjection
{
public:
	GaussProjection(OGRPoint center);
	~GaussProjection(void);
	void getGussCoord(OGRPoint& p);
private:
	OGRPoint centerPoint;
};

