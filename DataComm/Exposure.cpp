#include "StdAfx.h"
#include "Exposure.h"
#include <process.h> 
#include "dos.h"
#include <windows.h>
#include <sstream>
#include "SqliteManger.h"

using namespace std;
using std::stringstream;

/** �߳��˳���־ */   
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
	m_lastDistan = 9999999;
	m_pGlobalAirLine = CGlobalAirLine::GetInstance();

	CSystemParam::GetExposurParam( m_expParam );
	COMM_PARAM comp;
	CSystemParam::GetCameraCommParam(comp);
	m_ExposurePort.InitPort(comp.port, comp.baud);

	UDP_PARAM udpParam;
	CSystemParam::GetUDPParam(udpParam);
	m_udpServer.Init(udpParam.port);

/*
////////TEST
	GPRMC plane;
	plane.time = 600001;
	plane.az = 357.920000;
	plane.vel = 24.224160 ;
	plane.status = '1';
	plane.pos.high = 111.30;
	plane.pos.lat = 40.074283 ;//40.072800; 
	plane.pos.lon = 116.344158 ;//116.338220;//

	CURRENT_POINT currentPT;
	currentPT.position.high = 36;
	currentPT.position.lat =  40.074648 ;//40.081570;
	currentPT.position.lon =  116.344198 ;//116.338005;//116.337880;
	double distan2 = GetDistanFrom2Points(currentPT.position,plane.pos);
	double distan3 = GetDistanForCalcPoint(currentPT.position,plane.pos,plane.az,plane.vel/3.6);

	bool status    = false;
	double delayms = 0.0, distan = 0.0;
	status = GetExposureStatus( plane, currentPT, delayms, distan);

	m_sleepmsec = delayms / m_expParam.frequency * 1000;

	plane.time = 600001;
	plane.az = 173.960000;
	plane.vel = 29.131960 ;
	plane.status = '1';
	plane.pos.high = 58;
	plane.pos.lat = 40.078755; 
	plane.pos.lon = 116.338203;//;116.338220;//116.337992;//116.338027;// 116.338680; // 116.338680 ;

	status = GetExposureStatus( plane, currentPT, delayms, distan);
	

	//outPT = {lon=116.33867871830509 lat=40.072432822958994 

	///�н�
//	double angle  = GetAngleFrom2Points(outPT, ptend);
	///��ȡ������ڷɻ����к����ϵ�ͶӰ�롣
	double distan = GetDistanFrom2Points(ptend, outPT);

	COORDINATE ptend1,ptstart1; 
	ptstart1.lat = 35.434320 ;
	ptstart1.lon =  113.944445;
	ptend1.high = 200;
	ptend1.lat = 35.43420 ;
	ptend1.lon = 113.94450;

	double an1  = GetAngleFrom2Points(ptend1,ptstart1);
	double distan = GetDistanFrom2Points(ptend1,ptstart1);

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
					//COMM_MESSAGE msg = *(COMM_MESSAGE*)pBuffer;
					if(commMsg->msgtype == MSG_GPGGA)
					{
					//	IsNeedExposure(commMsg->body.position);
					}
					else if( commMsg->msgtype == MSG_GPVTG) 
					{
						m_lastVel = commMsg->body.velocity.vel;
						m_lastAZ  = commMsg->body.velocity.az;
					}
					else if(commMsg->msgtype == MSG_GPRMC)///λ����Ϣ
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
    /** �õ������ָ�� */   
    CExposure *pExp = reinterpret_cast<CExposure*>(pParam);
	return pExp->DPThdImp(pParam);
}

bool CExposure::OpenDataProcessThread()  
{ 
	 /** ����߳��Ƿ��Ѿ������� */   
    if (m_hDataProcessThread != INVALID_HANDLE_VALUE)  
    {  
        /** �߳��Ѿ����� */   
        return false;  
    }  
 
    s_bExit = false;  
    /** �߳�ID */   
    UINT threadId;  
    /** �����������ݼ����߳� */   
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
	if(status == '1' )
	{
		m_dataProcess.PackGPRMC(&pt, msg, m_lastTargetPT.lineIndex, m_lastTargetPT.pintIndex);
	}
	else
	{
		m_dataProcess.PackGPRMC(&pt, msg);
	}

	m_udpServer.SendData(msg.c_str(),msg.size());
}

bool CExposure::IsNeedExposure(GPRMC pt)
{
	m_pGlobalAirLine = CGlobalAirLine::GetInstance();
	CURRENT_POINT currentPT;
	m_pGlobalAirLine->GetCurrentPiont( currentPT );
	currentPT.status = false;

	///�Ѿ��ع����
	if( GetExposurePointStatus(currentPT.lineIndex, currentPT.pintIndex) )
	{
	    SendToUDP(pt,'0');
		return false;
	}

	m_lastTargetPT = currentPT;
	///���ƥ��״̬������Ҫ��
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
		string log = "׼���ع��У�";
		char cLOG[256];
		sprintf(cLOG,"���߱��%d �ع����%d ����%f γ��%f ����%f �ɻ�λ�� ����%f γ��%f �ٶ�%f ��λ��%f", 
			          currentPT.lineIndex,currentPT.pintIndex, currentPT.position.lon,currentPT.position.lat, 
					  distan, pt.pos.lon,pt.pos.lat,pt.vel,pt.az);
		log += cLOG;
		m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());
		SendToUDP(pt,'0');
		return false;
	}
	else
	{
		m_sleepmsec = delayms / m_expParam.frequency * 1000;
		SetEvent(m_hEvtExposure);

		AddExposurePoint(currentPT.lineIndex, currentPT.pintIndex);
		///��¼��־
		string log = "����ع⣺";
		char cLOG[256];
		sprintf(cLOG,"���߱��%d �ع����%d ����%f γ��%f �ع��ӳ�%d ����%f �ɻ�λ�� ����%f γ��%f �ٶ�%f ��λ��%f", 
			          currentPT.lineIndex,currentPT.pintIndex, currentPT.position.lon,currentPT.position.lat, 
					  m_sleepmsec, distan, pt.pos.lon,pt.pos.lat,pt.vel,pt.az);
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		//�����ݿ�
		CSqliteManger::GetInstance()->InsertExposure(pt,currentPT);

		///��UDP�����ع�״̬
		m_lastTargetPT.status = true;
		SendToUDP(pt, '1');
	}
	return true;
}
/*
bool CExposure::GetExposureStatus(const GPRMC &planeInfo, const CURRENT_POINT &currentPT, double & delayms, double & distan )
{
	distan  = GetDistanFrom2Points(planeInfo.pos, currentPT.position);
	///������ַɻ���ʼԶ���������������
	if( distan > m_lastDistan && distan < m_expParam.distan)
	{
		string log = "��ʼԶ���ع�㣺";
		char cLOG[256];
		sprintf(cLOG,"���߱��%d �ع����%d ����%f γ��%f �ع��ӳ�%d ����%f �ɻ�λ�� ����%f γ��%f �ٶ�%f ��λ��%f", 
			          currentPT.lineIndex,currentPT.pintIndex, currentPT.position.lon,currentPT.position.lat, 
					  m_sleepmsec, distan, planeInfo.pos.lon,planeInfo.pos.lat,planeInfo.vel,planeInfo.az);
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		m_lastDistan = 9999999;
		return true;
	}

	double angle = GetAngleFrom2Points(currentPT.position, planeInfo.pos );
	double dAngle = planeInfo.az - angle;

//	double delayDis   = dvel * m_expParam.delay /1000;///����ӳ�ʱ�䣬���о���(��ʱ������)
//  double scale = (distan - delayDis)/ nextSecDis / m_expParam.frequency ;
	double dvel = planeInfo.vel / 3.6 ;///�ٶȣ�m/s
	double nextSecDis = dvel / m_expParam.frequency ;  ///��һ��λ�õ��ϱ�ʱ���о���
    double calcDis = GetDistanFrom2Points(currentPT.position, planeInfo.pos, planeInfo.az, nextSecDis );
	if( calcDis > distan)
	{
		string log = "��ȷ�ع����ɹ���";
		char cLOG[256];
		sprintf(cLOG,"���ٶȣ�m/s��%f ���ع�����%f",dvel, distan );
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		delayms = distan / nextSecDis;
		m_lastDistan = 9999999;
		return true;
	}
	else
	{
		string log = "��ȷ�ع����ʧ�ܣ�";
		char cLOG[256];
		sprintf(cLOG,"���ٶȣ�m/s��%f ���ع�����%f",dvel, distan );
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());
		m_lastDistan = distan;
        return false;
	}
}

bool CExposure::GetExposureStatus(const GPRMC &planeInfo, const CURRENT_POINT &currentPT, double & delayms, double & distan )
{
	distan  = GetDistanFrom2Points(planeInfo.pos, currentPT.position);
	///������ַɻ���ʼԶ���������������
	if( distan > m_lastDistan)
	{
		string log = "��ʼԶ���ع�㣺";
		char cLOG[256];
		sprintf(cLOG,"���߱��%d �ع����%d ����%f γ��%f �ع��ӳ�%d ����%f �ɻ�λ�� ����%f γ��%f �ٶ�%f ��λ��%f", 
			          currentPT.lineIndex,currentPT.pintIndex, currentPT.position.lon,currentPT.position.lat, 
					  m_sleepmsec, distan, planeInfo.pos.lon,planeInfo.pos.lat,planeInfo.vel,planeInfo.az);
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		m_lastDistan = 9999999;
		return true;
	}

	double angle = GetAngleFrom2Points(currentPT.position, planeInfo.pos );
	double dAngle = planeInfo.az - angle;

//	double delayDis   = dvel * m_expParam.delay /1000;///����ӳ�ʱ�䣬���о���(��ʱ������)
//  double scale = (distan - delayDis)/ nextSecDis / m_expParam.frequency ;
	double dvel = planeInfo.vel / 3.6 * abs(cos(dAngle));///�ٶȣ�m/s
	double nextSecDis = dvel / m_expParam.frequency ;  ///��һ��λ�õ��ϱ�ʱ���о���

	if( nextSecDis > distan)
	{
		string log = "��ȷ�ع����ɹ���";
		char cLOG[256];
		sprintf(cLOG,"���ٶȣ�m/s��%f ���ع�����%f",dvel, distan );
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		delayms = distan / nextSecDis;
		m_lastDistan = 9999999;
		return true;
	}
	else
	{
		string log = "��ȷ�ع����ʧ�ܣ�";
		char cLOG[256];
		sprintf(cLOG,"���ٶȣ�m/s��%f ���ع�����%f",dvel, distan );
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());
		m_lastDistan = distan;
        return false;
	}
}
*/


bool CExposure::GetExposureStatus(const GPRMC &planeInfo, const CURRENT_POINT &currentPT, double & delayms, double & distan )
{
	distan  = GetDistanFrom2Points(planeInfo.pos, currentPT.position);
	delayms = 0;
	///������ַɻ���ʼԶ���������������
	if( distan > m_lastDistan && distan < m_expParam.distan)
	{
		string log = "�Ѿ�Զ���ع�㣺";
		char cLOG[256];
		sprintf(cLOG,"���߱��%d �ع����%d ����%f γ��%f ����%f �ɻ�λ�� ����%f γ��%f �ٶ�%f ��λ��%f", 
			          currentPT.lineIndex,currentPT.pintIndex, currentPT.position.lon,currentPT.position.lat, 
					  distan, planeInfo.pos.lon,planeInfo.pos.lat,planeInfo.vel,planeInfo.az);
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		m_lastDistan = 9999999;
		return true;
	}

	///��ȡͶӰ���˹����
	COORDINATE outPT;
	GetProjectionPt( planeInfo.pos, planeInfo.az, currentPT.position , outPT);
	double prj_X, prj_Y;
	prj_X = outPT.lon;
	prj_Y = outPT.lat;

	///��ǰʱ�̷ɻ���˹����
	double cpp_X, cpp_Y; 
	m_GEGeoCaculate.BL2XY_Gauss(planeInfo.pos.lon, planeInfo.pos.lat, cpp_X, cpp_Y);

	///Ԥ����һʱ�̷��е��˹����
	double dvel = planeInfo.vel / 3.6;///�ٶȣ�m/s
	double nextSecDis = dvel / m_expParam.frequency ;  ///��һ��λ�õ��ϱ�ʱ���о���
	double hAZ = GetHoriFromAZ(planeInfo.az)/180*PI;
	double next_Y = cpp_Y + nextSecDis * cos(hAZ);
	double next_X = cpp_X + nextSecDis * sin(hAZ);

	if( isContain(cpp_X,cpp_Y, next_X, next_Y, prj_X, prj_Y) )
	{
		//	double delayDis   = dvel * m_expParam.delay /1000;///����ӳ�ʱ�䣬���о���(��ʱ������)
        //  double scale = (distan - delayDis)/ nextSecDis / m_expParam.frequency ;
		///�ж�λ�ù�ϵ
	    double tmpdistan = sqrt( (prj_X - cpp_X)*(prj_X - cpp_X) + (prj_Y - cpp_Y)*(prj_Y - cpp_Y) );
		delayms = tmpdistan / nextSecDis;

		string log = "��ȷ�ع����ɹ���";
		char cLOG[256];
		sprintf(cLOG,"���ٶȣ�m/s��%f ���ع�����%f",dvel, distan );
		log += cLOG;		
	    m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

		m_lastDistan = 9999999;
		return true;
	}
	else
	{
        double nextdis = GetDistanForCalcPoint(currentPT.position, planeInfo.pos, planeInfo.az, nextSecDis );
		if( nextdis > distan && distan < m_expParam.distan )
		{
			string log = "��һ�����Զ���ع�㣬�����ع⣺";
			char cLOG[256];
			sprintf(cLOG,"���߱��%d �ع����%d ����%f γ��%f �ع��ӳ�%d ����%f �ɻ�λ�� ����%f γ��%f �ٶ�%f ��λ��%f", 
				currentPT.lineIndex,currentPT.pintIndex, currentPT.position.lon,currentPT.position.lat, 
				m_sleepmsec, distan, planeInfo.pos.lon,planeInfo.pos.lat,planeInfo.vel,planeInfo.az);
			log += cLOG;		
			m_pFile->GetInstance()->WriteLog(log.c_str(), log.length());

			m_lastDistan = 9999999;
		    return true;
		}
		else
		{
			m_lastDistan = 9999999;
			return false;
		}
	}
}

bool CExposure::isContain(double s_X, double s_Y, double e_X, double e_Y, double p_X, double p_Y)
{
	if((p_X > min(s_X, e_X)) && (p_X < max(s_X,e_X)) && 
		(p_Y > min(s_Y, e_Y)) && (p_Y < max(s_Y,e_Y)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

///armPT:Ŀ������� ccp���ɻ���ǰ������ az_B:�ɻ���ǰ����
///out_pt��Ŀ����ڷ���·���ϵ�ͶӰ����
void CExposure::GetProjectionPt(COORDINATE ccp, double az_B, COORDINATE armPT,  COORDINATE &out_pt )
{
	double a_X, a_Y, b_X, b_Y;
	m_GEGeoCaculate.BL2XY_Gauss(ccp.lon, ccp.lat, a_X, a_Y);
	m_GEGeoCaculate.BL2XY_Gauss(armPT.lon, armPT.lat, b_X, b_Y);

	double kA = 1/tan( GetHoriFromAZ(az_B) * PI / 180 );
	if (kA == 0) //����б�ʲ��������  
	{  
		out_pt.lon = a_X;  
		out_pt.lat = b_Y;  
	}  
	else  
	{
		out_pt.lon = (double)((kA * a_X + b_X / kA + b_Y - a_Y) / (1 / kA + kA));
		out_pt.lat = (double)(-1 / kA * (out_pt.lon - b_X) + b_Y); 
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

void CExposure::GetNextStepPt( COORDINATE ptA, double vl, double az, double freq, COORDINATE &cross )
{
}

/************************************************************************
Function:  GetAngleFrom2Points
Description: �ɵ�ͼ�ϵ���֪�����������ĽǶȹ�ϵ
Calls: 
Input:   ָ���ĵ�ͼ����������
Output:������ 
Return:   �Ƕ�ֵ
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
}

double CExposure::GetDistanForCalcPoint(COORDINATE linePt, COORDINATE cpp, double az, double distan)
{
	double s_X, s_Y, e_X, e_Y;
	m_GEGeoCaculate.BL2XY_Gauss(cpp.lon, cpp.lat, e_X, e_Y);
	double haz = GetHoriFromAZ(az)/180*PI;

	e_Y = e_Y + distan * cos(haz);
	e_X = e_X + distan * sin(haz);

	m_GEGeoCaculate.BL2XY_Gauss(linePt.lon, linePt.lat, s_X, s_Y);

	double dx = e_X - s_X;
	double dy = e_Y - s_Y;
	return sqrtf(dx * dx + dy * dy);
}

///������ع��
bool CExposure::AddExposurePoint(int lineIndex, int pointIndex)
{
	if( lineIndex > 0 && pointIndex > 0 )
	{
		int newint = JoinInt(lineIndex, pointIndex );
		m_pointList.push_back(newint);
		return true;
	}
	else
	{
		return false;
	}
}

///��ȡ�ع���Ƿ��ع�״̬
bool CExposure::GetExposurePointStatus(int lineIndex, int pointIndex)
{
	if( lineIndex > 0 && pointIndex > 0 )
	{
		int newint = JoinInt(lineIndex, pointIndex );
		std::list<int>::iterator iter = m_pointList.begin();
		for( ; iter != m_pointList.end(); iter++ )
		{
			if( *iter == newint )
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}

int CExposure::JoinInt(int front,int back)
{ 
	int nResult = 0; 
	char sBuf[20] = {'\0'};
	sprintf(sBuf, "%d%d", front, back);
	nResult = atoi(sBuf);
	return nResult;
}

bool CExposure::OpenListenThread()  
{  
    /** ����߳��Ƿ��Ѿ������� */   
    if (m_hListenThread != INVALID_HANDLE_VALUE)  
    {  
        /** �߳��Ѿ����� */   
        return false;  
    }  
 
    s_bListenExit = false;  
    /** �߳�ID */   
    UINT threadId;  
    /** �����������ݼ����߳� */   
    m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);  
    if (!m_hListenThread)  
    {  
        return false;  
    }  
    /** �����̵߳����ȼ�,������ͨ�߳� */   
    if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))  
    {  
        return false;  
    }  
 
    return true;  
} 

UINT WINAPI CExposure::ListenThread( void* pParam )  
{  
    /** �õ������ָ�� */   
    CExposure *pSerialPort = reinterpret_cast<CExposure*>(pParam);  
  
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