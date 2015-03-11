#include "StdAfx.h"
#include "DataProcess.h"


CDataProcess::CDataProcess(void)
{
}


CDataProcess::~CDataProcess(void)
{
}

///GPS定位信息：获取坐标 例如：
//$GPGGA,161229.487,3723.2475,N,12158.3416,W,1,07,1.0,9.0,M, , ,,0000*18
//纬度3723.2475 ddmm.mmmm    N 北纬
//经度12158.3416 dddmm.mmmm W 西经
//1无差分，定位有效，0是无效
//高度：9.0, 单位M
void CDataProcess::UnPackGPGGA( const string &data, GPGGA *pMsg )
{
	if ( data == "" )
	{
		return;
	}
	double lat, lon ;
	char cValue[10], flag;
	int num = 0, s_scale = 0, e_scale = 0;

	for( int i = 0; i < data.length(); i++)
	{
		if( data[i] != ',' )
		{
			continue;
		}

		num++;
		if ( num == 3)
		{
			e_scale = i;
			///纬度
			memset(cValue, 0, 10);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			lat = atof(cValue);
			pMsg->pos.lat = (int)lat % 100 + (lat - (int)lat % 100*100)/60;
		}
		else if( num == 4)
		{
		    memcpy(&flag, data.data() + s_scale, 1);
			if(flag == 'S')
	        {
		       pMsg->pos.lat = - pMsg->pos.lat;
	        }
		}
		else if ( num == 5)
		{
			e_scale = i;
			///经度
			memset(cValue, 0, 10);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			lon = atof(cValue);
	        pMsg->pos.lon = (int)lon % 100 + (lon - (int)lon % 100*100)/60;
		}
		else if( num == 6)
		{
		    memcpy(&flag, data.data() + s_scale, 1);
			if(flag == 'W')
	        {
		       pMsg->pos.lon = - pMsg->pos.lon;
	        }
		}
		else if( num == 7)
		{
	        ///定位状态
	        memcpy(&pMsg->status, data.data() + s_scale, 1);
		}
		else if ( num == 10)
		{
			e_scale = i;
			///高程
			memset(cValue, 0, 10);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->pos.high = atof(cValue);
			return;
		}
		s_scale = i + 1;
	}
}

///速度信息:获取航向和速度  例如
///$GPVTG,309.62,T, ,M,0.13,N,0.2,K*6E
void CDataProcess::UnPackGPVTG( const string &data, GPVTG *pMsg )
{
	if ( data == "")
	{
		return;
	}

	char cValue[12];
	int num = 0, s_scale = 0, e_scale = 0;

	for( int i = 0; i < data.length(); i++)
	{
		if( data[i] != ',' )
		{
			continue;
		}

		num++;
		if ( num == 2)
		{
			e_scale = i;
			///方位
			memset(cValue, 0, 12);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->az = atof(cValue);

			continue;
		}
		else if ( num == 6)
		{
			e_scale = i;
			///速度
			memset(cValue, 0, 12);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->vel = atof(cValue);

			return;
		}

		s_scale = i + 1;
	}
}