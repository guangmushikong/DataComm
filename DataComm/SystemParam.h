#pragma once

#include "datadefine.h"
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
                                std::list<GuidancePoint*>& lsGPs);

	void GetAirLinePTList( std::list<GuidancePoint*>& lsGPs );
private:
	static  EXPOSURE_PARAM      m_exposureParam;//曝光参数

	static std::list<GuidancePoint*> m_lsAirLinePTInfo;///航线信息
};

