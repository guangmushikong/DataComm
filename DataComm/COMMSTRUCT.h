#ifndef	_COMMSTRUCT_H
#define _COMMSTRUCT_H

/** 
* @brief	���̼���Ϣ�ӿڶ���
* @author	 
* @version	1.0
* @date		20150304
* @bug		��Ϣ�ӿڶ��廹������ȫȷ�������������
* @warning	N/A
* @note		N/A
*/

/**
*	��Ϣ���Ͷ���
*/
enum  COMM_MSGTYPE					
{
	///λ����Ϣ
	MSG_GPGGA = 1000,

	///�ٶ���Ϣ
	MSG_GPVTG,

	MSG_GPRMC
};

enum GuidancePointType
{
	AirPort,
	A1Type,
	A2Type,
	B1Type,
	B2Type,
	Normal
};
typedef struct _COORDINATE
{
	///���ȣ���λ�ȣ�����Ϊ��������Ϊ��
	double   lon;

	///ά�ȣ���λ�ȣ���γΪ������γΪ��
	double   lat;
    
	///���θ߶ȣ���λ��
	double   high;

}COORDINATE;

typedef struct _GPGGA
{
	char      status;
    
	COORDINATE pos;
}GPGGA;

typedef struct _GPVTG
{
	///�ٶȵ�λkm/h
	double   vel;

	///�����������нǣ���λ��
	double   az;
    
}GPVTG;

typedef struct _GPRMC
{
	///GPSʱ��
	double    time;

	///״̬
	char      status;
    
	COORDINATE pos;

		///�ٶȵ�λkm/h
	double   vel;

	///�����������нǣ���λ��
	double   az;
}GPRMC;
/**
*	��Ϣ�嶨��
*/
typedef union _COMM_MSGBODY
{
	///λ����Ϣ����
	GPGGA    position;

    ///�ٶ���Ϣ����
	GPVTG    velocity;

	///���λ��Ϣ
	GPRMC    position_info;

}COMM_MSGBODY;


/**
*	��Ϣ�ṹ����
*/
typedef struct _COMM_MESSAGE				
{
	///��Ϣͷ
	COMM_MSGTYPE	msgtype;

	///��Ϣ��
	COMM_MSGBODY	body;

}COMM_MESSAGE;

///��ǰ�������Ϣ
typedef struct _CURRENT_POINT				
{
	///����
	COORDINATE	position;

	///���߱��
	int lineIndex;

	///�������;
	int pintIndex;

	///�Ƿ����������
	bool status;
	 
	///���߽Ƕȣ�������
	double airline_az;

	bool distanceMatchFlag;
	bool headingMatchFlag;
	bool posingMatchFlag;
	bool topologyMatchFlag;
	bool airlineMatchFlag;

	///ƫ����
	double  drift_angle;

	///ǰ�������
	double  distance;

	GuidancePointType PointType;

	///ˮƽ���
	double  h_distance;

	///�߳����
	double  v_distance;

	_CURRENT_POINT()
	{
		status = false;
		lineIndex = 0;
        pintIndex = 0;
		airline_az = 0.0;
		PointType = AirPort;
		h_distance = 0.0;
        v_distance = 0.0;
	}

}CURRENT_POINT;

#endif