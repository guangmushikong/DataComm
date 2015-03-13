#pragma once

#include "datadefine.h"
//#include "MatchAirLinePoint.h"
#include <iostream>
 
#define CONFIG_AIRLINE_PATH_NAME "..\..\config"

///曝光控制参数
typedef struct _EXPOSURE_PARAM
{
	double		delay;//曝光延迟：单位毫秒

	double      distan;//距离容差，单位米

	double      angle;///拍摄角度容差，单位度
	
}EXPOSURE_PARAM;

class CSystemParam
{
public:
	CSystemParam(void);
	~CSystemParam(void);
public:
	 /**   
	 *  
	 *  读取系统参数
	 *  @param    fname 配置文件名
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void IniSysParam();
     /**
	 *  
	 *  相机曝光参数  
	 *  @param    expParam 曝光参数
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void GetExposurParam(EXPOSURE_PARAM &expParam);

	static void registerGhtFile(std::string filePath,
                                /*std::vector<GuidancePoint*>& vtrGPs*/
								std::map<std::string, GuidancePointPtr>& mapGPs);
	
	GuidancePoint getMatchingPointFromGPs(const COORDINATE& p);

	void getNextGPHead(const std::string& _head, std::string& nextHead);

	void GetAirLinePTList( std::vector<GuidancePoint*>& vtrGPs );
private:
	static  EXPOSURE_PARAM      m_exposureParam;//曝光参数

	//static std::vector<GuidancePoint*> m_vtrAirLinePTInfo;///航线信息
	static std::map<std::string, GuidancePointPtr> m_mapAirLinePTInfo;
	/*
	 * head is the key of m_mapAirLinePTInfo
	 * head consist of two parts, the default value is 01-0A1. 
	 * 1st is the line index(01), 2nd is the point index or point type(0A1)
	*/
	static std::string head;
	static BoundingBox* pBoundingBox;
};

