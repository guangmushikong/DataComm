#pragma once

#include "stdafx.h"
#include "IDataUpdate.h"
#include "DataContainer.h"
#include "COMMSTRUCT.h"
#include "SystemParam.h"
#include "SerialPort.h"
#include "GlobalAirLine.h"
#include <math.h>

//数学常数pei
#define  PI				3.1415926535898
#define  PIO2           1.5707963267948965        /* Pi/2 */
#define  TWOPI          6.283185307179586         /* 2*Pi  */
//椭球扁率倒数
#define  EARTHE84_1F          298.257223563
//WGS84坐标系基本参数
//地球椭球赤道半径(m)
#define  EARTHE84_A           6378137.0

class CExposure : public IDataUpdate
{
public:
	CExposure(void);
	~CExposure(void);
public:
	virtual void DataUpdate( char *pBuffer, int size );

private:
	void GetProjectionPt(double az_A, COORDINATE ptA, double az_B, COORDINATE ptB,  COORDINATE &out_pt );
	///以当前航向，当前速度行驶，下一次上报数据时，飞机的位置
	void GetNextStepPt( COORDINATE ptA, double vl, double az, double freq, COORDINATE &cross );

	///获取两点的方位角，正北夹角
	double GetAngleFrom2Points(COORDINATE ptend, COORDINATE ptstar);

	///获取两点距离
	double GetDistanFrom2Points(COORDINATE ptend, COORDINATE ptstart);

	bool IsNeedExposure(GPGGA pt);
public:
	UINT DPThdImp(LPVOID pParam);
	 /** 开启监听线程  
     *  
     *  本监听线程完成对串口数据的监听,并将接收到的数据打印到屏幕输出  
     *  @return: bool  操作是否成功  
     *  @note:   当线程已经处于开启状态时,返回flase  
     *  @see:      
     */ 
	bool OpenDataProcessThread();

	static UINT WINAPI DataProcessThread( void* pParam );

////串口监听
public:
	 /** 开启监听线程  
     *  
     *  本监听线程完成对串口数据的监听,并将接收到的数据打印到屏幕输出  
     *  @return: bool  操作是否成功  
     *  @note:   当线程已经处于开启状态时,返回flase  
     *  @see:      
     */ 
	bool OpenListenThread();

	 /** 等待拍照命令 
     *  
     *  监听数据处理线程的命令，收到命令，拍照  
     *  @param:  void * pParam 线程参数  
     *  @return: UINT WINAPI 线程返回值  
     *  @note:     
     *  @see:      
     */ 
    static UINT WINAPI ListenThread(void* pParam);  
public:

	HANDLE	m_hEvtsDPThread;

    /** 线程句柄 */   
    HANDLE  m_hDataProcessThread;  

	/** 线程退出标志变量 */   
    static bool s_bExit; 

private:
	CDataContainer<string*> *m_pContainer;

	///最新的速度
	double m_lastVel;

	///最新的航向
	double m_lastAZ;

	//最新的目标点
	CURRENT_POINT m_targetPT;

	EXPOSURE_PARAM m_expParam;

	///频率
	double m_frequency;

	//曝光延迟
	double m_delay;
private:
	CGlobalAirLine *m_pGlobalAirLine;

	///曝光串口对象
	CSerialPort m_ExposurePort;

	/** 线程句柄 */   
    HANDLE    m_hListenThread;  

	static bool s_bListenExit;

	HANDLE	m_hEvtExposure;

	///休息毫秒数
	DWORD m_sleepmsec;
};

