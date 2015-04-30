#include "StdAfx.h"
#include "Exposure.h"
#include <process.h> 
#include "dos.h"
#include <windows.h>
#include <sstream>
#include "SqliteManger.h"

using namespace std;
using std::stringstream;

/** 线程退出标志 */   
bool CExposure::s_bExit = false; 
bool CExposure::s_bListenExit = false;

CExposure::CExposure(void)
{
	m_pContainer = new CDataContainer<string*>();
	m_hEvtsDPThread = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hDataProcessThread = INVALID_HANDLE_VALUE;

	m_hEvtExposure = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hListenThread = INVALID_HANDLE_VALUE;
	OpenListenThread();
	m_sleepmsec = 0;
	m_pGlobalAirLine = CGlobalAirLine::GetInstance();

	CSystemParam::GetExposurParam( m_expParam );
	COMM_PARAM comp;
	CSystemParam::GetCameraCommParam(comp);
	m_ExposurePort.InitPort(comp.port, comp.baud);

	UDP_PARAM udpParam;
	CSystemParam::GetUDPParam(udpParam);
	m_udpServer.Init(udpParam.port);

////////TEST
	GPRMC plane;
	plane.time = 600001;
	plane.az = 60;
	plane.vel = 58;
	plane.status = '1';
	plane.pos.high = 58;
	plane.pos.lat = 39.861998  ;//40.072682 ;//40.072800; 
	plane.pos.lon = 116.204688  ;// 116.338680; // 116.338680 ;

	CURRENT_POINT currentPT;
	currentPT.position.high = 32.9;
	currentPT.position.lat =39.863103;
	currentPT.position.lon =  116.207170;

	bool status    = false;
	double delayms = 0.0, distan = 0.0;
	status = GetExposureStatus( plane, currentPT, delayms, distan);
	m_sleepmsec = delayms / m_expParam.frequency * 1000;

	//outPT = {lon=116.33867871830509 lat=40.072432822958994 

	///夹角
//	double angle  = GetAngleFrom2Points(outPT, ptend);
	///获取拍摄点在飞机飞行航线上的投影与。
//	double distan = GetDistanFrom2Points(ptend, outPT);
/*
	COORDINATE ptend1,ptstart1; 
	ptstart1.lat = 39.87711;
	ptstart1.lon =  116.23666;
	ptend1.high = 200;
	ptend1.lat = 39.87664 ;
	ptend1.lon = 116.23565;
	double an1  = GetAngleFrom2Points(ptend1,ptstart1);

	DWORD my1 = GetTickCount();
	Sleep(100);
		DWORD my2 = GetTickCount();
		DWORD nei = my2 - my1;
		int sd = nei;
	
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
	ptB.lat = 1;
	//GetProjectionPt(airline_AZ, ptA, pos_AZ, ptB, ptCross);

	GPRMC pMsg;
	pMsg.time = 600001;
	pMsg.az = 23.34;
	pMsg.vel = 123.432;
	pMsg.status = '1';
	pMsg.pos.high = 123.45;
	pMsg.pos.lat =40.0851966667; //40.074805; 
	pMsg.pos.lon =  116.34304;//116.33859;

	CURRENT_POINT currentPT;
	currentPT.lineIndex = 1;
	currentPT.pintIndex = 1;
	currentPT.position.lat = 45.4566;
	currentPT.position.lon = 123.234;
	currentPT.position.high = 23.1;
	CSqliteManger::GetInstance()->InsertExposure(pMsg,currentPT);
*/
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
					if(commMsg->msgtype == MSG_GPGGA)
					{
					//	IsNeedExposure(commMsg->body.position);
					}
					else if( commMsg->msgtype == MSG_GPVTG) 
					{
						m_lastVel = commMsg->body.velocity.vel;
						m_lastAZ  = commMsg->body.velocity.az;
					}
					else if(commMsg->msgtype == MSG_GPRMC)///位置信息
					{
						IsNeedExposure(commMsg->body.position_info);
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

void CExposure::SendToUDP(GPRMC pt, char status)
{
	string msg;
	pt.status = status;
	m_dataProcess.PackGPRMC(&pt, msg);
	m_udpServer.SendData(msg.c_str(),msg.size());
}

bool CExposure::IsNeedExposure(GPRMC pt)
{
	m_pGlobalAirLine = CGlobalAirLine::GetInstance();
	CURRENT_POINT currentPT;
	m_pGlobalAirLine->GetCurrentPiont( currentPT );
	currentPT.status = false;
	if( m_lastTargetPT.lineIndex == currentPT.lineIndex && m_lastTargetPT.pintIndex == currentPT.pintIndex && m_lastTargetPT.status == true )
	{
		SendToUDP(pt,'0');
		return false;
	}
	m_lastTargetPT = currentPT;

	if( !currentPT.distanceMatchFlag  || !currentPT.airlineMatchFlag || !currentPT.headingMatchFlag)
	{
        SendToUDP(pt,'0');
		return false;
	}

	bool status    = false;
	double delayms = 0.0, distan = 0.0;
	status = GetExposureStatus( pt, currentPT, delayms, distan);
	if( !status )
	{
		string log = "准备曝光中：";
		char cLOG[180];
		sprintf(cLOG,"航线编号%d 曝光点编号%d 经度%f 纬度%f 距离%f ", currentPT.lineIndex,currentPT.pintIndex,
			                   currentPT.position.lon,currentPT.position.lat, distan );
		log += cLOG;
		m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());
		SendToUDP(pt,'0');
		return false;
	}
	else
	{
		m_sleepmsec = delayms / m_expParam.frequency * 1000;
		SetEvent(m_hEvtExposure);

		///记录日志
		string log = "完成曝光：";
		char cLOG[180];
		sprintf(cLOG,"航线编号%d 曝光点编号%d 经度%f 纬度%f 曝光延迟%d 距离%f", currentPT.lineIndex,currentPT.pintIndex,
			                   currentPT.position.lon,currentPT.position.lat, m_sleepmsec, distan );
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		//存数据库
		CSqliteManger::GetInstance()->InsertExposure(pt,currentPT);

		///向UDP发送曝光状态
		m_lastTargetPT.status = true;
		SendToUDP(pt, '1');
	}
	return true;
}

bool CExposure::GetExposureStatus(const GPRMC &planeInfo, const CURRENT_POINT &currentPT, double & delayms, double & distan )
{
	COORDINATE outPT;
	GetProjectionPt( currentPT.position, planeInfo.az, planeInfo.pos, outPT);

	distan = GetDistanFrom2Points(planeInfo.pos, outPT);

	double dvel = planeInfo.vel / 3.6;///速度，m/s
	double nextSecDis = dvel / m_expParam.frequency ;  ///下一个位置点上报时飞行距离

//	double delayDis   = dvel * m_expParam.delay /1000;///相机延迟时间，飞行距离(暂时不考虑)
//  double scale = (distan - delayDis)/ nextSecDis / m_expParam.frequency ;

	if( nextSecDis > distan)
	{
		delayms = distan / nextSecDis;
		return true;
	}
	else
	{
        return false;
	}
}

///ptA:拍摄点坐标 ptB：飞机当前点坐标 az_B:飞机当前航向
///out_pt：目标点在飞行路线上的投影坐标
void CExposure::GetProjectionPt(COORDINATE ptA, double az_B, COORDINATE ptB,  COORDINATE &out_pt )
{
	double za =  GetHoriFromAZ(az_B);
	double kA = tan( GetHoriFromAZ(az_B) * PI / 180 );
	if (kA == 0) //垂线斜率不存在情况  
	{  
		out_pt.lon = ptA.lon;  
		out_pt.lat = ptB.lat;  
	}  
	else  
	{  
		out_pt.lon = (double)((kA * ptA.lon + ptB.lon / kA + ptB.lat - ptA.lat) / (1 / kA + kA));  
		out_pt.lat = (double)(-1 / kA * (out_pt.lon - ptB.lon) + ptB.lat);  
	}  
}
double CExposure::GetHoriFromAZ(double az_A )
{
   if( az_A >= 90 && az_A <= 180)
   {
	   return 270 + (180 - az_A );
   }
   else
   {
	   double tmp = 90 - az_A;
	   return tmp>0?tmp:(tmp+360);
   }
}
/*
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
*/
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
	double s_X, s_Y, e_X, e_Y;
	m_GEGeoCaculate.BL2XY_Gauss(ptend.lon, ptend.lat, e_X, e_Y);
	m_GEGeoCaculate.BL2XY_Gauss(ptstart.lon, ptstart.lat, s_X, s_Y);

	double dx = e_X - s_X;
	double dy = e_Y - s_Y;
	return sqrtf(dx * dx + dy * dy);
	/*
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
	*/
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
				unsigned char flag[16];
				memcpy(flag, "1              6", 16);
				pSerialPort->m_ExposurePort.WriteData(flag, 16);
				pSerialPort->m_pGlobalAirLine->SetPointStatus(pSerialPort->m_targetPT.lineIndex, pSerialPort->m_targetPT.pintIndex,true);
			}
		}
    }  
 
    return 0;  
}  