#include<stdio.h>
#include"Datadecoding.h"
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include "matrix.h"
#define reF -4.442807633e-10
#pragma warning(disable:4996)



/*****************************************************

	根据接收到的Data计算应该得到的crc值

******************************************************/
unsigned int crc32(const unsigned char * buff, int len)
{
	int i, j;
	unsigned int crc = 0;

	for (i=0; i < len; i++)
	{
		crc ^= buff[i];
		for (j = 0; j < 8; j++)
		{
			//与1按位与等于0说明是偶数，等于1说明是奇数
			if (crc & 1)//奇数右移一位并与POLYCRC32按位异或
			{
				crc = (crc >> 1) ^ POLYCRC32;
			}
			//偶数右移一位;
			else    
			{
				crc >>= 1;
			}
		}
	}
	return crc;
}

void IONrenew(unsigned char *buff, int HeaderLgth , IONUTC *ION)
{
	for (int i = 0; i < 4; i++)
	{
		ION->alpha[i] = R8(buff +HeaderLgth+ 8 * i);
		ION->belta[i] = R8(buff + HeaderLgth + 8 * i + 32);
	}
}

void OmestarPOSrenew(unsigned char * buff,int HeaderLgth, PSRPOS * blh)
{
	blh->POStime.Week = Ushort2(buff + 14);
	blh->POStime.SecOfWeek = Ulong4(buff + 16)/1000;
	blh->POSblh.B = R8(buff + HeaderLgth + 8);
	blh->POSblh.L = R8(buff + HeaderLgth + 16);
	blh->POSblh.H= R8(buff + HeaderLgth + 24) - R4(buff + HeaderLgth + 32);
}

void GPSObsGet(unsigned char * buff, int HeaderLgth, RawData *rawdata)
{
	int obsnum = Long4(buff + HeaderLgth);
	rawdata->Obsnum = obsnum;
	//rawdata->Obs->TIME.Week= Ushort2(buff + 14);
	//rawdata->Obs->TIME.SecOfWeek = Ulong4(buff + 16) / 1000;
	for (int i = 0; i < obsnum; i++)
	{
		//原本是下面这么写，数组越界有份;
		//rawdata[i].Obs->PRN = Ushort2(buff + HeaderLgth +44*i+ 4);
		rawdata->Obs[i].TIME.Week = Ushort2(buff + 14);
		rawdata->Obs[i].TIME.SecOfWeek = Ulong4(buff + 16) / 1000;
		rawdata->Obs[i].PRN = Ushort2(buff + HeaderLgth + 44 * i + 4);
		rawdata->Obs[i].psr = R8(buff + HeaderLgth + 44 * i + 8);
		rawdata->Obs[i].adr = R8(buff + HeaderLgth + 44 * i + 20);
		rawdata->Obs[i].dopp = R4(buff + HeaderLgth + 44 * i + 32);
		rawdata->Obs[i].CN0 = R4(buff + HeaderLgth + 44 * i + 36);
		rawdata->Obs[i].locktime = R4(buff + HeaderLgth + 44 * i + 40);
	}

}

void GPSEPHEMrenew(unsigned char * buff, int HeaderLgth, RawData*rawdata)
{
	unsigned long PRN = Ulong4(buff + HeaderLgth);
	rawdata->GPSEPH[PRN - 1].PRN = PRN;
	rawdata->GPSEPH[PRN - 1].EPHEMtime.Week= Ushort2(buff + 14);
	rawdata->GPSEPH[PRN - 1].EPHEMtime.SecOfWeek = Ulong4(buff + 16) / 1000;
	rawdata->GPSEPH[PRN - 1].health= Ulong4(buff +HeaderLgth+12) ;
	rawdata->GPSEPH[PRN - 1].IODE1 = Ulong4(buff + HeaderLgth + 16);
	rawdata->GPSEPH[PRN - 1].IODE2 = Ulong4(buff + HeaderLgth + 20);
	rawdata->GPSEPH[PRN - 1].week = Ulong4(buff + HeaderLgth + 24);
	rawdata->GPSEPH[PRN - 1].z_week = Ulong4(buff + HeaderLgth + 28);
	rawdata->GPSEPH[PRN - 1].toe = R8(buff + HeaderLgth + 32);
	rawdata->GPSEPH[PRN - 1].A = R8(buff + HeaderLgth + 40);
	rawdata->GPSEPH[PRN - 1].delta_N = R8(buff + HeaderLgth + 48);
	rawdata->GPSEPH[PRN - 1].M0 = R8(buff + HeaderLgth + 56);
	rawdata->GPSEPH[PRN - 1].ecc = R8(buff + HeaderLgth + 64);
	rawdata->GPSEPH[PRN - 1].w = R8(buff + HeaderLgth + 72);
	rawdata->GPSEPH[PRN - 1].cuc = R8(buff + HeaderLgth + 80);
	rawdata->GPSEPH[PRN - 1].cus = R8(buff + HeaderLgth + 88);
	rawdata->GPSEPH[PRN - 1].crc = R8(buff + HeaderLgth + 96);
	rawdata->GPSEPH[PRN - 1].crs = R8(buff + HeaderLgth + 104);
	rawdata->GPSEPH[PRN - 1].cic = R8(buff + HeaderLgth + 112);
	rawdata->GPSEPH[PRN - 1].cis = R8(buff + HeaderLgth + 120);
	rawdata->GPSEPH[PRN - 1].I0 = R8(buff + HeaderLgth + 128);
	rawdata->GPSEPH[PRN - 1].IDOT= R8(buff + HeaderLgth + 136);
	rawdata->GPSEPH[PRN - 1].w0 = R8(buff + HeaderLgth + 144);
	rawdata->GPSEPH[PRN - 1].rate_w0 = R8(buff + HeaderLgth + 152);
	rawdata->GPSEPH[PRN - 1].iodc = Ulong4(buff + HeaderLgth + 160);
	rawdata->GPSEPH[PRN - 1].toc = R8(buff + HeaderLgth + 164);
	rawdata->GPSEPH[PRN - 1].tgd = R8(buff + HeaderLgth + 172);
	rawdata->GPSEPH[PRN - 1].a10 = R8(buff + HeaderLgth + 180);
	rawdata->GPSEPH[PRN - 1].a11 = R8(buff + HeaderLgth + 188);
	rawdata->GPSEPH[PRN - 1].a12 = R8(buff + HeaderLgth + 196);
	rawdata->GPSEPH[PRN - 1].N = R8(buff + HeaderLgth + 208);
	rawdata->GPSEPH[PRN - 1].URA= R8(buff + HeaderLgth + 216);
}

bool CalulateSVPositionAndSVclock(GPSTIME Time, unsigned long PRN, RawData *rawdata,SVPositinAndSpeed* pvofsv, double* delta_tsv_L1, double* delta_tsv_L1pie)
{
	if ((PRN > 0) && (PRN < 33) && rawdata->GPSEPH[PRN - 1].health == 0)
	{
		pvofsv->PRN = PRN;
		double A = rawdata->GPSEPH[PRN - 1].A;
		double tk = 604800 * (Time.Week - rawdata->GPSEPH[PRN - 1].week) + Time.SecOfWeek - rawdata->GPSEPH[PRN - 1].toe;
		if (fabs(tk) < 7200)
		{
			double n0 = sqrt(u / (A*A*A));
			double n = n0 + rawdata->GPSEPH[PRN - 1].delta_N;
			double Mk = rawdata->GPSEPH[PRN - 1].M0 + n*tk;
			double Ek0 = Mk;
			double Ek1;
			for (int i = 0; i < 20; i++)
			{
				Ek1 = Mk + rawdata->GPSEPH[PRN - 1].ecc*sin(Ek0);
				if (fabs(Ek0 - Ek1) < 0.00000000000000001)
				{
					break;
				}
				else
				{
					Ek0 = Ek1;
				}
			}
			double Ek = Ek1;
			double vk = atan2(sqrt(1 - rawdata->GPSEPH[PRN - 1].ecc*rawdata->GPSEPH[PRN - 1].ecc)*sin(Ek)*(1 - rawdata->GPSEPH[PRN - 1].ecc*cos(Ek)), (cos(Ek) - rawdata->GPSEPH[PRN - 1].ecc)*(1 - rawdata->GPSEPH[PRN - 1].ecc*cos(Ek)));
			double phik = vk + rawdata->GPSEPH[PRN - 1].w;//计算升交点角距;
			//计算二阶调和改正数;
			double delta_uk = rawdata->GPSEPH[PRN - 1].cus*sin(2 * phik) + rawdata->GPSEPH[PRN - 1].cuc*cos(2 * phik);
			double delta_rk = rawdata->GPSEPH[PRN - 1].crs*sin(2 * phik) + rawdata->GPSEPH[PRN - 1].crc*cos(2 * phik);
			double delta_ik = rawdata->GPSEPH[PRN - 1].cis*sin(2 * phik) + rawdata->GPSEPH[PRN - 1].cic*cos(2 * phik);
			double uk = phik + delta_uk;
			double rk = A*(1 - rawdata->GPSEPH[PRN - 1].ecc*cos(Ek)) + delta_rk;
			double ik = rawdata->GPSEPH[PRN - 1].I0 + delta_ik + rawdata->GPSEPH[PRN - 1].IDOT*tk;
			double xkpie = rk*cos(uk);
			double ykpie = rk*sin(uk);
			double wk = rawdata->GPSEPH[PRN - 1].w0 + (rawdata->GPSEPH[PRN - 1].rate_w0 - wedot)*tk - wedot*rawdata->GPSEPH[PRN - 1].toe;
			pvofsv->SVPositin.X = xkpie*cos(wk) - ykpie*cos(ik)*sin(wk);
			pvofsv->SVPositin.Y = xkpie*sin(wk) + ykpie*cos(ik)*cos(wk);
			pvofsv->SVPositin.Z = ykpie*sin(ik);

			//EKDOT这个公式给错了，应该是n;
			double EKDOT = n / (1 - rawdata->GPSEPH[PRN - 1].ecc*cos(Ek));
			double phikDOT = sqrt((1 + rawdata->GPSEPH[PRN - 1].ecc) / (1 - rawdata->GPSEPH[PRN - 1].ecc))*pow(cos(vk / 2), 2) / (pow(cos(Ek / 2), 2))*EKDOT;
			double ukDOT = 2 * (rawdata->GPSEPH[PRN - 1].cus*cos(2 * phik) - rawdata->GPSEPH[PRN - 1].cuc*sin(2 * phik))*phikDOT + phikDOT;
			double rkDOT = A*rawdata->GPSEPH[PRN - 1].ecc*sin(Ek)*EKDOT + 2 * (rawdata->GPSEPH[PRN - 1].crs*cos(2 * phik) - rawdata->GPSEPH[PRN - 1].crc*sin(2 * phik))*phikDOT;
			double ikDOT = rawdata->GPSEPH[PRN - 1].IDOT + 2 * (rawdata->GPSEPH[PRN - 1].cis*cos(2 * phik) - rawdata->GPSEPH[PRN - 1].cic*sin(2 * phik))*phikDOT;
			double wkDOT = rawdata->GPSEPH[PRN - 1].rate_w0 - wedot;
			matrix RDOT;
			RDOT.col = 4;
			RDOT.row = 3;
			RDOT.num = 12;
			RDOT.element = (double*)malloc(sizeof(double)*RDOT.num);
			RDOT.element[0] = cos(wk);
			RDOT.element[1] = -sin(wk)*cos(ik);
			RDOT.element[2] = -(xkpie*sin(wk) + ykpie*cos(wk)*cos(ik));
			RDOT.element[3] = ykpie*sin(wk)*sin(ik);
			RDOT.element[4] = sin(wk);
			RDOT.element[5] = cos(wk)*cos(ik);
			RDOT.element[6] = xkpie*cos(wk) - ykpie*sin(wk)*cos(ik);
			RDOT.element[7] = ykpie*cos(wk)*sin(ik);
			RDOT.element[8] = 0;
			RDOT.element[9] = sin(ik);
			RDOT.element[10] = 0;
			RDOT.element[11] = ykpie*cos(ik);

			double xkpieDOT = rkDOT*cos(uk) - rk*ukDOT*sin(uk);
			double ykpieDOT = rkDOT*sin(uk) + rk*ukDOT*cos(uk);
			matrix xkpieDOTykpieDOTwkDOTikDOT;
			xkpieDOTykpieDOTwkDOTikDOT.col = 1;
			xkpieDOTykpieDOTwkDOTikDOT.row = 4;
			xkpieDOTykpieDOTwkDOTikDOT.num = 4;
			xkpieDOTykpieDOTwkDOTikDOT.element = (double*)malloc(sizeof(double)*xkpieDOTykpieDOTwkDOTikDOT.num);
			xkpieDOTykpieDOTwkDOTikDOT.element[0] = xkpieDOT;
			xkpieDOTykpieDOTwkDOTikDOT.element[1] = ykpieDOT;
			xkpieDOTykpieDOTwkDOTikDOT.element[2] = wkDOT;
			xkpieDOTykpieDOTwkDOTikDOT.element[3] = ikDOT;
			//matrix xkDOTykDOTzkDOT = matrixmultiply(RDOT, xkpieDOTykpieDOTwkDOTikDOT);

			matrix xkDOTykDOTzkDOT;
			xkDOTykDOTzkDOT.row = RDOT.row; xkDOTykDOTzkDOT.col = xkpieDOTykpieDOTwkDOTikDOT.col; xkDOTykDOTzkDOT.num = xkDOTykDOTzkDOT.col*xkDOTykDOTzkDOT.row;
			xkDOTykDOTzkDOT.element= (double*)malloc(sizeof(double)*xkDOTykDOTzkDOT.num);
			MatrixMultiply(&RDOT, &xkpieDOTykpieDOTwkDOTikDOT, &xkDOTykDOTzkDOT);
			pvofsv->SVSpeed.Xdot = xkDOTykDOTzkDOT.element[0];
			pvofsv->SVSpeed.Ydot = xkDOTykDOTzkDOT.element[1];
			pvofsv->SVSpeed.Zdot = xkDOTykDOTzkDOT.element[2];
			free(RDOT.element);
			free(xkpieDOTykpieDOTwkDOTikDOT.element);
			free(xkDOTykDOTzkDOT.element);
			double ClkBias = rawdata->GPSEPH[PRN - 1].a10;
			double ClkDrift = rawdata->GPSEPH[PRN - 1].a11;
			double ClkDriftRate = rawdata->GPSEPH[PRN - 1].a12;
			double toc = rawdata->GPSEPH[PRN - 1].toc;
			double TGD = rawdata->GPSEPH[PRN - 1].tgd;


			//double GM = u;
			//double guangse = c;
			//double F = -2 * sqrt(GM) / (guangse*guangse);
			//double ff = -2 * sqrt(u) / (c*c);


			double delta_tr = reF*rawdata->GPSEPH[PRN - 1].ecc*sqrt(A)*sin(Ek);
			double t_toc = (Time.SecOfWeek - toc) + 604800 * (Time.Week - rawdata->GPSEPH[PRN - 1].week);
			*delta_tsv_L1 = ClkBias + ClkDrift*t_toc + ClkDriftRate*t_toc*t_toc + delta_tr - rawdata->GPSEPH[PRN - 1].tgd;
			double delta_trpie = reF*rawdata->GPSEPH[PRN - 1].ecc*sqrt(A)*cos(Ek)*EKDOT;
			*delta_tsv_L1pie = ClkDrift + 2 * ClkDriftRate*t_toc + delta_trpie;

			return true;
		}
		else 
		{
			return false;
		}
	}
	else 
	{ 
		return false;
	}
}


//BL传弧度，A,E传弧度，返回值是电离层延迟(以米为单位的！);
double Klobutchar(GPSTIME time,BLH blh, double E, double A, IONUTC ION)
{
	if ((blh.H > 500000)|| (blh.H < -1000)||((ION.alpha[0]==0)&& (ION.alpha[1] == 0) && (ION.alpha[2] == 0) && (ION.alpha[3] == 0) && (ION.belta[0] == 0) && (ION.belta[1] == 0) && (ION.belta[2] == 0) && (ION.belta[3] == 0))) return 0;
	else
	{
		double psi = 0.0137 / (E / PI + 0.11) - 0.022;//以半周为单位;
		double phiI = blh.B / PI + psi*cos(A);//以半周为单位;
		if (phiI > 0.416) phiI = 0.416;
		else if (phiI < -0.416) phiI = -0.416;
		double lambdaI = blh.L / PI + psi*sin(A) / cos(phiI*PI);
		double phim = phiI + 0.064*cos((lambdaI - 1.617)*PI);
		double t = 43200 * lambdaI + time.SecOfWeek;
		do
		{
			if (t > 86400)
			{
				t = t - 86400;
			}
			else if (t < 0)
			{
				t = t + 86400;
			}
		} while ((t < 0 )||(t > 86400));
		double AI = ION.alpha[0] + ION.alpha[1] * phim + ION.alpha[2] * phim*phim + ION.alpha[3] * phim*phim*phim;
		if (AI < 0) AI = 0;
		double Pion = ION.belta[0] + ION.belta[1] * phim + ION.belta[2] * phim*phim + ION.belta[3] * phim*phim*phim;
		if (Pion < 72000) Pion = 72000;
		double XI = 2 * PI*(t - 50400) / Pion;//radians
		double F = 1.0 + 16.0*(0.53 - E/PI)*(0.53 - E/PI)*(0.53 - E/PI);
		double IL1GPS = 0.0;
		if (fabs(XI) < 1.57)
		{
			IL1GPS = (5e-9 + (ION.alpha[0] + ION.alpha[1] * phim + ION.alpha[2] * phim*phim+ ION.alpha[3] * phim*phim*phim)*(1 - XI*XI / 2 + XI*XI*XI*XI / 24))*F;
		}
		else
		{
			IL1GPS = 5e-9*F;
		}
		return (IL1GPS*c);
	}
}

double Hopfield(double H, double E)//E以度数传入！！;
{
	if ((H > 17000) || (H < -1000)) return 0;
	else
	{
		double RH0 = 0.5;
		double p0 = 1013.25;
		//double T0 = 15.0;
		double T0 = 15.0 + 273.15;
		double H0 = 0.0;
		double RH = RH0*exp(-0.0006396*(H - H0));
		double p = p0*pow((1 - 0.0000226*(H - H0)), 5.225);
		double T = T0 - 0.0065*(H - H0);
		double e_Hopfield = RH*exp(-37.2465 + 0.213166*T - 0.000256908*T*T);
		double hw = 11000.0;
		double hd = 40136 + 148.72*(T0 - 273.16);
		double Kw = 155.2e-7 * 4810 * e_Hopfield*(hw - H) / (T*T);
		double Kd = 155.2e-7*p*(hd - H) / T;
		double delta_Trop = Kd / (sin(sqrt(E*E + 6.25) / 180 * PI)) + Kw / (sin(sqrt(E*E + 2.25) / 180 * PI));
		return delta_Trop;
	}
}

bool IsGPSEPHEM(unsigned long PRN)
{
	if ((PRN > 0) && (PRN < 33))  return true;
	else return false;
}

bool GPSEPHEMCanBeUsed(RawData * rawdata, unsigned long PRN,int i)
{
	if ((PRN > 0) && (PRN < 33))
	{
		double tk = 604800 * (rawdata->Obs[i].TIME.Week - rawdata->GPSEPH[PRN - 1].week) + rawdata->Obs[i].TIME.SecOfWeek - rawdata->GPSEPH[PRN - 1].toe;
		if (fabs(tk) < 7200)
		{
			return true;
		}
		else return false;
	}
	else return false;
}

bool RTCMCanBeUsed(RTCM1 * R, unsigned long PRN,RawData * rawdata)
{
	if ((R->rtcm1body[PRN-1].s == 0)&&R->rtcm1body[PRN-1].AOD==rawdata->GPSEPH[PRN-1].IODE1)
	{
		return true;
	}
	else
	{
		return false;
	}
}



double calculateE(XYZ* SV,XYZ* Re)
{
	ENU SVENU;
	XYZToENU(SV, Re, &SVENU);
	double aa = atan(SVENU.U / (sqrt(SVENU.E*SVENU.E + SVENU.N*SVENU.N)));
	return aa;
}


double calculateA(XYZ * SV, XYZ * Re)
{
	ENU SVENU;
	XYZToENU(SV, Re, &SVENU);
	//if(SVNEU.E>0) return(acos(SVNEU.N / (sqrt(SVNEU.E*SVNEU.E + SVNEU.N*SVNEU.N))));
	//else return(acos(SVNEU.N / (sqrt(SVNEU.E*SVNEU.E + SVNEU.N*SVNEU.N))))+PI;
	return(atan2(SVENU.E, SVENU.N));
}



/******处理double数据**********/
double R8(unsigned char * p)
{
	double r;
	memcpy(&r, p, 8);
	return r;
}

/******处理float数据**********/
float R4(unsigned char * p)
{
	float r;
	memcpy(&r, p, 4);
	return r;
}

/*****处理unsigned int 数据***/
unsigned int Uint4(unsigned char * p)
{
	unsigned int r;
	memcpy(&r, p, 4);
	return r;
}

/*****处理unsigned long 数据******/
unsigned long Ulong4(unsigned char * p)
{
	unsigned long r;
	memcpy(&r, p, 4);
	return r;
}

/************处理unsigned short 数据***************/
unsigned short Ushort2(unsigned char * p)
{
	unsigned short r;
	memcpy(&r, p, 2);
	return r;
}

short Short2(unsigned char * p)
{
    short r;
	memcpy(&r, p, 2);
	return r;
}

/************处理int 数据***************/
int Int4(unsigned char * p)
{
	int r;
	memcpy(&r, p, 4);
	return r;
}

long Long4(unsigned char * p)
{
	long r;
	memcpy(&r, p, 4);
	return r;
}
