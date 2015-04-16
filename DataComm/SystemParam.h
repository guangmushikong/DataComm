#pragma once

#include "datadefine.h"
//#include "MatchAirLinePoint.h"
#include <iostream>
#include "GuidancePointMatch.h"
 
#define CONFIG_AIRLINE_PATH_NAME ".\\config\\kkk.ght"

#define CONFIGFILENAME  ".\\config\\Datacom.cfg"

///�ع���Ʋ���
typedef struct _EXPOSURE_PARAM
{
	double		delay;//�ع��ӳ٣���λ����

	double      distan;//�����ݲ��λ��

	double      angle;///����Ƕ��ݲ��λ��

	double      trigger;//������ؿ���ʱ���λ���룬������رտ�������ʱ��� 

	int         frequency;//GPS���ջ�Ƶ�ʣ���������λ��HZ��ÿ������λ�����ݸ���

	double      rate; //�ع�İٷֱ�.�������ĳ�������ع���ʴﵽ��ֵ�������л�����
	
}EXPOSURE_PARAM;

///��������
typedef struct _COMM_PARAM
{
	int		port;//���ں�

	int     baud;//�����ݲ��λ��
}COMM_PARAM;

///UDPͨ��
typedef struct _UDP_PARAM
{
	int		port;//���ں�
}UDP_PARAM;

class CSystemParam
{
public:
	CSystemParam(void);
	~CSystemParam(void);
public:
	 /**   
	 *  
	 *  ��ȡϵͳ����
	 *  @param    fname �����ļ���
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void IniSysParam();
     /**
	 *  
	 *  ����ع����  
	 *  @param    expParam �ع����
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void GetExposurParam(EXPOSURE_PARAM &param);

	 /**
	 *  
	 *  ��ȡ������ڲ���  
	 *  @param    param ���ڲ���
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void GetCameraCommParam(COMM_PARAM &param);
	/**
	 *  
	 *  ��ȡGPS���ջ����ڲ���  
	 *  @param    param ���ڲ���
	 *  @return   N/A  
	 *  @see      N/A
	 *  @note     N/A 
	 */ 
	static void GetGpsCommParam(COMM_PARAM &param);
		/**
	 *  
	 *  ��ȡUDP����  
	 *  @param    param UDP����
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
	static  EXPOSURE_PARAM      m_exposureParam;//�ع����

	//������ڲ���
	static COMM_PARAM m_cameraCommParam;

	//GPS���ջ����ڲ���
	static COMM_PARAM m_gpsCommParam;

	static GuidancePointMatch GP_Match;

	static UDP_PARAM m_udpParam;
};

