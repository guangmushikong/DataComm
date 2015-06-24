#ifndef	_COMMSTRUCT_H
#define _COMMSTRUCT_H

/** 
* @brief	进程间消息接口定义
* @author	 
* @version	1.0
* @date		20150304
* @bug		消息接口定义还不能完全确定，还需调整。
* @warning	N/A
* @note		N/A
*/

/**
*	消息类型定义
*/
enum  COMM_MSGTYPE					
{
	///位置信息
	MSG_GPGGA = 1000,

	///速度信息
	MSG_GPVTG,

	MSG_GPRMC
};


typedef struct _COORDINATE
{
	///经度，单位度，东经为正，西经为负
	double   lon;

	///维度，单位度，北纬为正，南纬为负
	double   lat;
    
	///海拔高度，单位米
	double   high;
}COORDINATE;

typedef struct _GPGGA
{
	char      status;
    
	COORDINATE pos;
}GPGGA;

typedef struct _GPVTG
{
	///速度单位km/h
	double   vel;

	///航向，与正北夹角，单位度
	double   az;
    
}GPVTG;

typedef struct _GPRMC
{
	///GPS时间
	double    time;

	///状态
	char      status;
    
	COORDINATE pos;

		///速度单位km/h
	double   vel;

	///航向，与正北夹角，单位度
	double   az;
}GPRMC;
/**
*	消息体定义
*/
typedef union _COMM_MSGBODY
{
	///位置信息数据
	GPGGA    position;

    ///速度信息数据
	GPVTG    velocity;

	///最简定位信息
	GPRMC    position_info;

}COMM_MSGBODY;


/**
*	消息结构定义
*/
typedef struct _COMM_MESSAGE				
{
	///消息头
	COMM_MSGTYPE	msgtype;

	///消息体
	COMM_MSGBODY	body;

}COMM_MESSAGE;

///当前拍摄点信息
typedef struct _CURRENT_POINT				
{
	///坐标
	COORDINATE	position;

	///航线编号
	int lineIndex;

	///拍摄点编号;
	int pintIndex;

	///是否已拍摄完成
	bool status;
	 
	///航线角度：与正北
	double airline_az;

	bool distanceMatchFlag;
	bool headingMatchFlag;
	bool posingMatchFlag;
	bool topologyMatchFlag;
	bool airlineMatchFlag;

	///偏航角
	double  drift_angle;

	///前方点距离
	double  distance;

	int PointType;

	///水平误差
	double  h_distance;

	///高程误差
	double  v_distance;

}CURRENT_POINT;

#endif