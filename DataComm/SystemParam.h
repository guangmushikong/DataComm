#pragma once

#include "datadefine.h"
//#include "MatchAirLinePoint.h"
#include <iostream>
#include "GuidancePointMatch.h"
 
#define CONFIG_AIRLINE_PATH_NAME ".\\config\\kkk.ght"

#define CONFIGFILENAME  ".\\config\\Datacom.cfg"

///曝光控制参数
typedef struct _EXPOSURE_PARAM
{
	double		delay;//曝光延迟：单位毫秒

	double      distan;//距离容差，单位米

	double      angle;///拍摄角度容差，单位度

	double      trigger;//相机开关控制时差，单位毫秒，即打开与关闭控制器的时间差 

	int         frequency;//GPS接收机频率，整数。单位：HZ，每秒生成位置数据个数

	double      rate; //曝光的百分比.代表如果某条航向曝光比率达到该值，即可切换航线
	
}EXPOSURE_PARAM;

///串口设置
typedef struct _COMM_PARAM
{
	int		port;//串口号

	int     baud;//距离容差，单位米
}COMM_PARAM;

///UDP通信
typedef struct _UDP_PARAM
{
	int		port;//串口号
}UDP_PARAM;

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
		/**
	 *  
	 *  获取UDP参数  
	 *  @param    param UDP参数
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void GetUDPParam(UDP_PARAM &param);

	static void getMatchedGP(GuidancePoint& tgrGP, GPRMC plane)
	{
		GP_Match.getMatchedGP(tgrGP, plane);
	}
private:
	static  EXPOSURE_PARAM      m_exposureParam;//曝光参数

	//相机串口参数
	static COMM_PARAM m_cameraCommParam;

	//GPS接收机串口参数
	static COMM_PARAM m_gpsCommParam;

	static GuidancePointMatch GP_Match;

	static UDP_PARAM m_udpParam;
};

