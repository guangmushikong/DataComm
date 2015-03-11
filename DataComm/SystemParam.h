#pragma once

#include "datadefine.h"
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
                                std::list<GuidancePoint*>& lsGPs);

	void GetAirLinePTList( std::list<GuidancePoint*>& lsGPs );
private:
	static  EXPOSURE_PARAM      m_exposureParam;//�ع����

	static std::list<GuidancePoint*> m_lsAirLinePTInfo;///������Ϣ
};

