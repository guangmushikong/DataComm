#include "StdAfx.h"
#include "DataProcess.h"


CDataProcess::CDataProcess(void)
{
}


CDataProcess::~CDataProcess(void)
{
}

///GPS��λ��Ϣ����ȡ���� ���磺
//$GPGGA,161229.487,3723.2475,N,12158.3416,W,1,07,1.0,9.0,M, , ,,0000*18
//γ��3723.2475 ddmm.mmmm    N ��γ
//����12158.3416 dddmm.mmmm W ����
//1�޲�֣���λ��Ч��0����Ч
//�߶ȣ�9.0, ��λM
void CDataProcess::UnPackGPGGA( const string &data, GPGGA *pMsg )
{
	if ( data == "" )
	{
		return;
	}
	double lat, lon ;
	char cValue[16], flag;
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
			///γ��
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			lat = atof(cValue);
			pMsg->pos.lat = (int)lat / 100 + (lat - (int)lat / 100*100)/60;
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
			///����
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			lon = atof(cValue);
	        pMsg->pos.lon = (int)lon / 100 + (lon - (int)lon / 100*100)/60;
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
	        ///��λ״̬
	        memcpy(&pMsg->status, data.data() + s_scale, 1);
		}
		else if ( num == 10)
		{
			e_scale = i;
			///�߳�
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->pos.high = atof(cValue);
			return;
		}
		s_scale = i + 1;
	}
}

///�ٶ���Ϣ:��ȡ������ٶ�  ����
///$GPVTG,309.62,T, ,M,0.13,N,0.2,K*6E
void CDataProcess::UnPackGPVTG( const string &data, GPVTG *pMsg )
{
	if ( data == "")
	{
		return;
	}

	char cValue[16];
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
			///��λ
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->az = atof(cValue);

			continue;
		}
		else if ( num == 6)
		{
			e_scale = i;
			///�ٶ�
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->vel = atof(cValue);

			return;
		}

		s_scale = i + 1;
	}
}
//$GPRMC,161229.487,A,3928.0398,N,11633.1530,E,23.13,45,120598,,*10	

void CDataProcess::UnPackGPRMC( const string &data, GPRMC *pMsg )
{
	if ( data == "" )
	{
		return;
	}
	double lat, lon ;
	char cValue[16], flag;
	int num = 0, s_scale = 0, e_scale = 0;

	for( int i = 0; i < data.length(); i++)
	{
		if( data[i] != ',' )
		{
			continue;
		}

		num++;
		if(s_scale == i)
		{
			continue;
		}

		if( num == 3)
		{
	        ///��λ״̬
	        memcpy(&pMsg->status, data.data() + s_scale, 1);
		}
		else if ( num == 4)
		{
			e_scale = i;
			///γ��
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			lat = atof(cValue);
			pMsg->pos.lat = (int)lat / 100 + (lat - (int)lat / 100*100)/60;
		}
		else if( num == 5)
		{
		    memcpy(&flag, data.data() + s_scale, 1);
			if(flag == 'S')
	        {
		       pMsg->pos.lat = - pMsg->pos.lat;
	        }
		}
		else if ( num == 6)
		{
			e_scale = i;
			///����
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			lon = atof(cValue);
	        pMsg->pos.lon = (int)lon / 100 + (lon - (int)lon / 100*100)/60;
		}
		else if( num == 7)
		{
		    memcpy(&flag, data.data() + s_scale, 1);
			if(flag == 'W')
	        {
		       pMsg->pos.lon = - pMsg->pos.lon;
	        }
		}
		else if ( num == 8)
		{
			e_scale = i;
			///�ٶ�(knots)
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->vel = atof(cValue);
			pMsg->vel *= 1.852;
		}
		else if ( num == 9)
		{
			e_scale = i;
			///��λ
			memset(cValue, 0, 16);
	        memcpy(&cValue, data.data() + s_scale, e_scale - s_scale);
			pMsg->az = atof(cValue);

			return;
		}
		s_scale = i + 1;
	}
}