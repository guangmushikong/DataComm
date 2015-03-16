#pragma once

#include "datadefine.h"
//#include "MatchAirLinePoint.h"
#include <iostream>
 
#define CONFIG_AIRLINE_PATH_NAME "..\..\config"

#define CONFIGFILENAME  ".\\config\\Datacom.cfg"

///曝光控制参数
typedef struct _EXPOSURE_PARAM
{
	double		delay;//曝光延迟：单位毫秒

	double      distan;//距离容差，单位米

	double      angle;///拍摄角度容差，单位度

	double      trigger;//相机开关控制时差，单位毫秒，即打开与关闭控制器的时间差 
	
}EXPOSURE_PARAM;

///串口设置
typedef struct _COMM_PARAM
{
	int		port;//串口号

	int     baud;//距离容差，单位米
}COMM_PARAM;

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
	static void GetExposurParam(EXPOSURE_PARAM &param);

	static void registerGhtFile(std::string filePath,
                                /*std::vector<GuidancePoint*>& vtrGPs*/
								std::map<std::string, GuidancePointPtr>& mapGPs);
	
	GuidancePoint getMatchingPointFromGPs(const COORDINATE& p);

	void getNextGPHead(const std::string& _head, std::string& nextHead);

	void GetAirLinePTList( std::vector<GuidancePoint*>& vtrGPs );

	 /**
	 *  
	 *  获取相机串口参数  
	 *  @param    param 串口参数
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void GetCameraCommParam(COMM_PARAM &param);
	/**
	 *  
	 *  获取GPS接收机串口参数  
	 *  @param    param 串口参数
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void GetGpsCommParam(COMM_PARAM &param);
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

	//相机串口参数
	static COMM_PARAM m_cameraCommParam;

	//GPS接收机串口参数
	static COMM_PARAM m_gpsCommParam;
};

