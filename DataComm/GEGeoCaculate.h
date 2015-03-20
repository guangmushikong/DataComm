#pragma once

#define PI 3.1415926535898

class CGEGeoCaculate
{
public:
	CGEGeoCaculate(void);
	~CGEGeoCaculate(void);

public:
	void CPrjPoint_WGS84();
	///longitude:输入参数，经度。latitude：输入参数，纬度
	///gauss_x:输出参数，高斯坐标X值，gauss_y:输出参数，高斯坐标Y值，
	bool BL2XY_Gauss(double longitude,double latitude, double &gauss_x, double &gauss_y );//将大地坐标投影到高斯上

	bool SetL0(double dL0);

private:
	double L0; // 中央子午线经度
	double B, L; // 大地坐标
	double x, y; // 高斯投影平面坐标
protected:
	double a, f; // 基本椭球参数  UTM只用到了这两个
	double b;//短半轴 0215
	double n;//n=(a-b)/(a+b) 0215
	double e2,e12;   //高斯
	double A1, A2, A3, A4; // 用于计算X的椭球参数  //高斯
};

