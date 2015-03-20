#include "StdAfx.h"
#include "GEGeoCaculate.h"


CGEGeoCaculate::CGEGeoCaculate(void)
{
	CPrjPoint_WGS84();
	L0 = 113.00 * 0.0174532925199433;
}


CGEGeoCaculate::~CGEGeoCaculate(void)
{
}

void CGEGeoCaculate::CPrjPoint_WGS84()
{
	a = 6378137;
	b = 6356752.3142;
	f = 1.0/298.257223563;
    e2 = 1 - ((f - 1) / f) * ((f - 1) / f);   
    e12 = (f / (f - 1)) * (f / (f - 1)) - 1;   
    n=(a-b)/(a+b);   
    A1 = a*(1-n+(pow(n,2)-pow(n,3))*5/4+(pow(n,4)-pow(n,5))*81/64)*PI/180;   
    A2 = (n-pow(n,2)+(pow(n,3)-pow(n,4))*7/8+pow(n,5)*55/64)*3*a/2;   
    A3 = (pow(n,2)-pow(n,3)+(pow(n,4)-pow(n,5))*3/4)*15*a/16;   
    A4 = (pow(n,3)-pow(n,4)+pow(n,5)*11/16)*35*a/48;   
}

bool CGEGeoCaculate::BL2XY_Gauss(double longitude,double latitude, double &gauss_x, double &gauss_y )  //已通过测试，WGS84坐标系统
{
	double longitude1,latitude1, longitude0,latitude0, X0,Y0, xval,yval;
	//double a,f; 
	double e2,ee, NN, T,C,A, M, iPI;
	iPI = 0.0174532925199433; ////3.1415926535898/180.0; 

	latitude  = latitude  * iPI;
	longitude = longitude * iPI;
	//a=6378245.0; f=1.0/298.3; //54年北京坐标系参数 
	////a=6378140.0; f=1/298.257; //80年西安坐标系参数 
	//ProjNo = (int)(longitude / ZoneWide) ; 
	//longitude0 = ProjNo * ZoneWide + ZoneWide / 2; 
	//longitude0 = longitude0 * iPI ;
	//latitude0=0; 
	//longitude1 = longitude * iPI ; //经度转换为弧度
	//latitude1 = latitude * iPI ; //纬度转换为弧度
	e2=2*f-f*f;
	ee=e2*(1.0-e2);
	NN=a/sqrt(1.0-e2*sin(latitude)*sin(latitude));
	T=tan(latitude)*tan(latitude);
	C=ee*cos(latitude)*cos(latitude);
	A=(longitude - this->L0 ) * cos(latitude);
	M=a*((1-e2/4-3*e2*e2/64-5*e2*e2*e2/256)*latitude - (3*e2/8+3*e2*e2/32+45*e2*e2
		*e2/1024) * sin(2 * latitude)
		+(15*e2*e2/256+45*e2*e2*e2/1024)
		*sin(4*latitude)-(35*e2*e2*e2/3072)*sin(6*latitude));
	xval = NN*(A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*ee)*A*A*A*A*A/120);
	//	xval = (A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*ee)*A*A*A*A*A/120);

	yval = M+NN*tan(latitude)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
		+(61-58*T+T*T+600*C-330*ee)*A*A*A*A*A*A/720);
	X0 = 500000; 
	Y0 = 0; 
	xval = xval+X0; yval = yval+Y0; 

	gauss_x = yval;
	gauss_y = xval;
	// cout.precision(12);
	// cout<<"xval:"<<this->x<<"\tyval:"<<this->y<<endl;

	return true;
}

bool CGEGeoCaculate::SetL0(double dL0)
{
	L0 = dL0;   //L0为弧度

	return true;
}