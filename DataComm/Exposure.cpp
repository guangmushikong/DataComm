#include "StdAfx.h"
#include "Exposure.h"
#include <process.h> 
#include "dos.h"
#include <windows.h>

/** 线程退出标志 */   
bool CExposure::s_bExit = false; 
bool CExposure::s_bListenExit = false;

CExposure::CExposure(void)
{
	m_pContainer = new CDataContainer<string*>();
	m_hEvtsDPThread = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hDataProcessThread = INVALID_HANDLE_VALUE;

	m_hEvtExposure = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ExposurePort.InitPort(3, 1200);
	m_hListenThread = INVALID_HANDLE_VALUE;
	OpenListenThread();
	m_sleepmsec = 0;

	m_pGlobalAirLine = CGlobalAirLine::GetInstance();


	COORDINATE ptend,ptstart;
	ptend.lat = 39.972390*PI/180;
	ptend.lon = 116.397689*PI/180;
	ptend.high = 200;
	ptstart.lat = 39.972798*PI/180;
	ptstart.lon = 116.398530*PI/180;
	ptstart.high = 79;
	double dis = GetDistanFrom2Points(ptend,ptstart);
	double an = GetAngleFrom2Points(ptend,ptstart);

	DWORD my1 = GetTickCount();
	Sleep(100);
		DWORD my2 = GetTickCount();
		DWORD nei = my2 - my1;
		int sd = nei;
	/*
	CSystemParam::GetExposurParam(m_expParam);
	double airline_AZ;
	double pos_AZ;
	double point_lat, point_lon;
	double pos_lat, pint_lon;
	COORDINATE ptA, ptB, ptCross;
	ptA.lon = 0;
	ptA.lat = 0;
	airline_AZ = 1;
	pos_AZ = 2;
	ptB.lon = 0;
	ptB.lat = 1;*/
	//GetProjectionPt(airline_AZ, ptA, pos_AZ, ptB, ptCross);


}


CExposure::~CExposure(void)
{
}

void CExposure::DataUpdate( char *pBuffer, int size )
{	
	if ( size != sizeof(COMM_MESSAGE) )
	{
		return;
	}	
	
	string* lpData = new string;
	lpData->append( pBuffer, size);
	COMM_MESSAGE msg = *(COMM_MESSAGE*)pBuffer;
	m_pContainer->PushData( lpData );

	SetEvent(m_hEvtsDPThread);	
	/*
		COMM_MESSAGE lpData = *(COMM_MESSAGE*)pBuffer;	
	double airline_AZ;
	double pos_AZ;
	double point_lat, point_lon;
	double pos_lat, pint_lon;
	COORDINATE ptA, ptB, ptCross;
	ptA.lon = 0;
	ptA.lat = 0;
	airline_AZ = 1;
	pos_AZ = 2;
	ptB.lon = 0;
	ptB.lat = 1;
	IsNeedExposure(lpData.body.position);
	GetProjectionPt(atan(airline_AZ), ptA, atan(pos_AZ), ptB, ptCross);
	*/
}

UINT CExposure::DPThdImp( LPVOID pParam )
{	
	string *pstrMsg;
	while ( !s_bExit  )
	{
		DWORD ret = WaitForSingleObject(m_hEvtsDPThread, -1 );

		switch (ret)
		{
		case WAIT_OBJECT_0:
			{						
				ResetEvent(m_hEvtsDPThread);
				
				while (m_pContainer->GetData(pstrMsg))
				{
					COMM_MESSAGE *commMsg = (COMM_MESSAGE* )pstrMsg->c_str();
					if(commMsg->msgtype == MSG_GPRMC)
					{
						IsNeedExposure(commMsg->body.position);
					}
					else if( commMsg->msgtype == MSG_GPVTG) 
					{
						m_lastVel = commMsg->body.velocity.vel;
						m_lastAZ  = commMsg->body.velocity.az;
					}
				}
			}
		}
	}
	return 1;
}

UINT WINAPI CExposure::DataProcessThread( void* pParam )  
{  
    /** 得到本类的指针 */   
    CExposure *pExp = reinterpret_cast<CExposure*>(pParam);
	return pExp->DPThdImp(pParam);
}

bool CExposure::OpenDataProcessThread()  
{ 
	 /** 检测线程是否已经开启了 */   
    if (m_hDataProcessThread != INVALID_HANDLE_VALUE)  
    {  
        /** 线程已经开启 */   
        return false;  
    }  
 
    s_bExit = false;  
    /** 线程ID */   
    UINT threadId;  
    /** 开启串口数据监听线程 */   
    m_hDataProcessThread = (HANDLE)_beginthreadex(NULL, 0, DataProcessThread, this, 0, &threadId);  
    if (!m_hDataProcessThread)  
    {  
        return false;  
    }   
    return true;  
}  

bool CExposure::IsNeedExposure(GPGGA pt)
{
//	SetEvent(m_hEvtExposure);
	m_pGlobalAirLine = CGlobalAirLine::GetInstance();
	CURRENT_POINT currentPT;
	m_pGlobalAirLine->GetCurrentPiont( currentPT );
	m_targetPT = currentPT;

	COORDINATE outPT;
	GetProjectionPt(currentPT.airline_az, currentPT.position, m_lastAZ, pt.pos, outPT);
	double angle = GetAngleFrom2Points(outPT, pt.pos);

	//std::cout << "\r\nIsNeedExposure:over \r\n" ; 
	if( abs(angle - m_lastAZ ) > 90 )
	{
		return false;
	}
	else
	{
		double distan = GetDistanFrom2Points(pt.pos, outPT);
		double nextSecDis = m_lastVel / m_frequency ;  ///下一个位置点上报时飞行距离
		double delayDis   =  m_lastVel * m_delay /1000;///相机延迟时间，飞行距离
		if( distan > ( nextSecDis + delayDis ) )
		{
			return false;
		}
		else
		{
			double scale = (distan - delayDis)/ nextSecDis / m_frequency ;///延迟这些秒为最近距离，开始拍照
			if(scale < 0 )
			{
				scale = 0;
			}
			m_sleepmsec = scale;
			SetEvent(m_hEvtExposure);
		}
	}
	return true;
}
	
///az_A:航线方向 az_B:飞行方向斜率
///ptA:目标点坐标 ptB当前点坐标
///out_pt目标点在飞行路线上的投影坐标
void CExposure::GetProjectionPt(double az_A, COORDINATE ptA, double az_B, COORDINATE ptB,  COORDINATE &out_pt )
{
	double kA = tan( az_A );
	double kB = tan( az_B );
	double x0, y0;
    if( az_A == az_B)
	{
		if( az_A == 90)
		{
			x0 = ptB.lon;
			y0 = ptA.lat;
		}
		else
		{
			x0 = (kB*ptB.lon + ptA.lon/kA + ptA.lat - ptB.lat) / (kB + 1/kA);
		    y0 = kB * (x0 - ptB.lon ) + ptB.lat;
		}
	}
	else if( kA != kB)
	{
		COORDINATE cross;
		cross.lon = (kA*ptA.lon + ptB.lat - ptA.lat - kB*ptB.lon)/(kA - kB);
		cross.lat = kA*(cross.lon - ptA.lon) + ptA.lat;

		double distan = sqrt( (ptB.lon - cross.lon) * (ptB.lon - cross.lon) + (ptB.lat - cross.lat) * (ptB.lat - cross.lat) );
		double angle0 = abs( az_A - az_B );
		double newX =  distan * cos( angle0 ) * cos( az_A );
		double newY =  distan * cos( angle0 ) * sin( az_A );

		x0 = newX + cross.lon ;
		y0 = newY + cross.lat;
	}
	out_pt.lat = y0;
	out_pt.lon = x0;
}

void CExposure::GetNextStepPt( COORDINATE ptA, double vl, double az, double freq, COORDINATE &cross )
{
}

/************************************************************************
Function:  GetAngleFrom2Points
Description: 由地图上的已知两点获得两点间的角度关系
Calls: 
Input:   指定的地图上任意两点
Output:　　　 
Return:   角度值
Author:   sunyuefeng                                                  
************************************************************************/
double CExposure::GetAngleFrom2Points(COORDINATE ptend, COORDINATE ptstar)
{
	double deltax =  ptend.lon - ptstar.lon;
	double deltay =  ptend.lat - ptstar.lat;
    double angle;
    if (deltax == 0)
    {
		if (deltay > 0)
		{
			angle = 0; 
		}
		else
		{
			angle = 180;
		}
	}
	else
	{
		angle = atan(deltay / deltax) * (180 / PI);
		if (deltax > 0 && deltay > 0 || deltax > 0 && deltay <= 0 )
		{
			angle = 90 - angle;
		}
		if (deltax < 0 && deltay >= 0 || deltax < 0 && deltay < 0)
		{
			angle = 270 - angle;
		}
	}
	return angle;
}

double CExposure::GetDistanFrom2Points(COORDINATE ptend, COORDINATE ptstart)
{
	double   n = 0.0, fai = 0.0, tmp = 0.0, e2 = 0.0, f1 = 0.0;
	double   sinf = 0.0;
	double   geod[3];
    geod[0]  = ptend.lat ;
	geod[1]  = ptend.lon;
	geod[2]  = ptend.high;

	if (geod[0] >PIO2 || geod[0] < -PIO2 || geod[1]>TWOPI || geod[1]<-PI )
	{
		return -20;
	}
	fai = geod[0];
	sinf = sin(fai);
	f1 = 1.0/EARTHE84_1F;
	e2 = 2.0 * f1 -f1 * f1; 
	tmp = sqrt(1.0 - e2 * sinf*sinf);
	n =  EARTHE84_A /tmp;
	
	double cart[3];
	cart[0] = (n + geod[2]) * cos(fai) *cos(geod[1]);
	cart[1] = (n + geod[2]) * cos(fai) *sin(geod[1]);
	cart[2] = (n *(1.0 - e2) + geod[2]) * sin(fai);
	
	///获取位置
	double dis = sqrt(cart[0]*cart[0] + cart[1]*cart[1] +cart[2]*cart[2]);

	double Err[2];
	Err[0] = (float) (dis * (ptstart.lat - geod[0]));
	Err[1] = (float) (dis * (ptstart.lon - geod[1]));

	return sqrt(Err[0]*Err[0] + Err[1]*Err[1] );
}

bool CExposure::OpenListenThread()  
{  
    /** 检测线程是否已经开启了 */   
    if (m_hListenThread != INVALID_HANDLE_VALUE)  
    {  
        /** 线程已经开启 */   
        return false;  
    }  
 
    s_bListenExit = false;  
    /** 线程ID */   
    UINT threadId;  
    /** 开启串口数据监听线程 */   
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);  
    if (!m_hListenThread)  
    {  
        return false;  
    }  
    /** 设置线程的优先级,高于普通线程 */   
    if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))  
    {  
        return false;  
    }  
 
    return true;  
} 

UINT WINAPI CExposure::ListenThread( void* pParam )  
{  
    /** 得到本类的指针 */   
    CExposure *pSerialPort = reinterpret_cast<CExposure*>(pParam);  
 
    // 线程循环,轮询方式读取串口数据  
    while (!pSerialPort->s_bListenExit)   
    {  
		DWORD ret = WaitForSingleObject(pSerialPort->m_hEvtExposure, -1 );

		switch (ret)
		{
		case WAIT_OBJECT_0:
			{
				ResetEvent(pSerialPort->m_hEvtExposure);
				Sleep(pSerialPort->m_sleepmsec);
				unsigned char flag[8];
				memcpy(flag, "1      6", 8);
				pSerialPort->m_ExposurePort.WriteData(flag, 8);
				pSerialPort->m_pGlobalAirLine->SetPointStatus(pSerialPort->m_targetPT.lineIndex, pSerialPort->m_targetPT.pintIndex,true);
			}
		}
    }  
 
    return 0;  
}  