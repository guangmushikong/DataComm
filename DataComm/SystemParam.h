#pragma once

#include "datadefine.h"
//#include "MatchAirLinePoint.h"
#include <iostream>
 
#define CONFIG_AIRLINE_PATH_NAME "..\..\config"

///�ع���Ʋ���
typedef struct _EXPOSURE_PARAM
{
	double		delay;//�ع��ӳ٣���λ����

	double      distan;//�����ݲ��λ��

	double      angle;///����Ƕ��ݲ��λ��
	
}EXPOSURE_PARAM;

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
	static void GetExposurParam(EXPOSURE_PARAM &expParam);

	static void registerGhtFile(std::string filePath,
                                /*std::vector<GuidancePoint*>& vtrGPs*/
								std::map<std::string, GuidancePointPtr>& mapGPs);
	
	GuidancePoint getMatchingPointFromGPs(const COORDINATE& p);

	void getNextGPHead(const std::string& _head, std::string& nextHead);

	void GetAirLinePTList( std::vector<GuidancePoint*>& vtrGPs );
private:
	static  EXPOSURE_PARAM      m_exposureParam;//�ع����

	//static std::vector<GuidancePoint*> m_vtrAirLinePTInfo;///������Ϣ
	static std::map<std::string, GuidancePointPtr> m_mapAirLinePTInfo;
	/*
	 * head is the key of m_mapAirLinePTInfo
	 * head consist of two parts, the default value is 01-0A1. 
	 * 1st is the line index(01), 2nd is the point index or point type(0A1)
	*/
	static std::string head;
	static BoundingBox* pBoundingBox;
};

