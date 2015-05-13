#pragma once

#include "stdafx.h"
#include "IDataUpdate.h"
#include "DataContainer.h"
#include "COMMSTRUCT.h"
#include "SystemParam.h"
#include "SerialPort.h"
#include "GlobalAirLine.h"
#include <math.h>
#include "GaussProjection.h"
#include "GEGeoCaculate.h"
#include "UDPServer.h"
#include "LogFile.h"

//��ѧ����pei
#define  PI				3.1415926535898
#define  PIO2           1.5707963267948965        /* Pi/2 */
#define  TWOPI          6.283185307179586         /* 2*Pi  */
//������ʵ���
#define  EARTHE84_1F          298.257223563
//WGS84����ϵ��������
//�����������뾶(m)
#define  EARTHE84_A           6378137.0

class CExposure : public IDataUpdate
{
	friend class GuidancePointMatch;
public:
	CExposure(void);
	~CExposure(void);
public:
	virtual void DataUpdate( char *pBuffer, int size );

private:
	void GetProjectionPt( COORDINATE ptA, double az_B, COORDINATE ptB,  COORDINATE &out_pt );
	///�Ե�ǰ���򣬵�ǰ�ٶ���ʻ����һ���ϱ�����ʱ���ɻ���λ��
	void GetNextStepPt( COORDINATE ptA, double vl, double az, double freq, COORDINATE &cross );

	///��ȡ����ķ�λ�ǣ������н�
	static double GetAngleFrom2Points(COORDINATE ptend, COORDINATE ptstar);

	///��ȡ�������
	double GetDistanFrom2Points(COORDINATE ptend, COORDINATE ptstart);

	bool IsNeedExposure(GPRMC pt);

	bool GetExposureStatus(const GPRMC &planeInfo, const CURRENT_POINT &currentPT, double & delayms, double & distan);

	///�ӷ�λ�ǻ�ȡˮƽ��
	double GetHoriFromAZ(double az_A );

	void SendToUDP(GPRMC pt, char status);
public:
	UINT DPThdImp(LPVOID pParam);
	 /** ���������߳�  
     *  
     *  �������߳���ɶԴ������ݵļ���,�������յ������ݴ�ӡ����Ļ���  
     *  @return: bool  �����Ƿ�ɹ�  
     *  @note:   ���߳��Ѿ����ڿ���״̬ʱ,����flase  
     *  @see:      
     */ 
	bool OpenDataProcessThread();

	static UINT WINAPI DataProcessThread( void* pParam );

	///������ع��
	bool AddExposurePoint(int lineIndex, int pointIndex);

	///��ȡ�ع���Ƿ��ع�״̬
	bool GetExposurePointStatus(int lineIndex, int pointIndex);

	int JoinInt(int front,int back);
////���ڼ���
public:
	 /** ���������߳�  
     *  
     *  �������߳���ɶԴ������ݵļ���,�������յ������ݴ�ӡ����Ļ���  
     *  @return: bool  �����Ƿ�ɹ�  
     *  @note:   ���߳��Ѿ����ڿ���״̬ʱ,����flase  
     *  @see:      
     */ 
	bool OpenListenThread();

	 /** �ȴ��������� 
     *  
     *  �������ݴ����̵߳�����յ��������  
     *  @param:  void * pParam �̲߳���  
     *  @return: UINT WINAPI �̷߳���ֵ  
     *  @note:     
     *  @see:      
     */ 
    static UINT WINAPI ListenThread(void* pParam);  
public:

	HANDLE	m_hEvtsDPThread;

    /** �߳̾�� */   
    HANDLE  m_hDataProcessThread;  

	/** �߳��˳���־���� */   
    static bool s_bExit; 
private:
	CDataContainer<string*> *m_pContainer;

	///���µ��ٶ�
	double m_lastVel;

	///���µĺ���
	double m_lastAZ;

	//���µ�Ŀ���
	CURRENT_POINT m_targetPT;

	CURRENT_POINT m_lastTargetPT;

	EXPOSURE_PARAM m_expParam;

	///Ƶ��
	double m_frequency;

	//�ع��ӳ�
	double m_delay;

	//UDP����
	CUDPServer m_udpServer; 

	CDataProcess m_dataProcess;
private:
	CGlobalAirLine *m_pGlobalAirLine;

	///�ع⴮�ڶ���
	CSerialPort m_ExposurePort;

	/** �߳̾�� */   
    HANDLE    m_hListenThread;  

	static bool s_bListenExit;

	HANDLE	m_hEvtExposure;

	///��Ϣ������
	int m_sleepmsec;

	CSystemParam  m_syfParam;

	CGEGeoCaculate m_GEGeoCaculate;

	CLogFile  *m_pFile;

	std::list<int>  m_pointList;
};

