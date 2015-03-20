#pragma once

#define PI 3.1415926535898

class CGEGeoCaculate
{
public:
	CGEGeoCaculate(void);
	~CGEGeoCaculate(void);

public:
	void CPrjPoint_WGS84();
	///longitude:������������ȡ�latitude�����������γ��
	///gauss_x:�����������˹����Xֵ��gauss_y:�����������˹����Yֵ��
	bool BL2XY_Gauss(double longitude,double latitude, double &gauss_x, double &gauss_y );//���������ͶӰ����˹��

	bool SetL0(double dL0);

private:
	double L0; // ���������߾���
	double B, L; // �������
	double x, y; // ��˹ͶӰƽ������
protected:
	double a, f; // �����������  UTMֻ�õ���������
	double b;//�̰��� 0215
	double n;//n=(a-b)/(a+b) 0215
	double e2,e12;   //��˹
	double A1, A2, A3, A4; // ���ڼ���X���������  //��˹
};

