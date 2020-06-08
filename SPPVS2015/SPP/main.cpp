#include<stdio.h>
#include"Datadecoding.h"
#include"Timechange.h"
#include"Referencechange.h"
#include "Positioning.h"
#include <stdlib.h>
#include "ReadFile.h"
#define CRTDBG_MAP_ALLOC  
#include <crtdbg.h>

#pragma warning(disable:4996)


int main()
{
	//为了调查内存泄漏具体使用看https://blog.csdn.net/Bryantaoli/article/details/96508705
	//_CrtSetBreakAlloc(169);
	/*CommonTime xs;
	GPSTIME xsgps;
	MJDTIME xsmjd;
	xs.Year = 2019;
	xs.Month = 4;
	xs.Day = 28;
	xs.Hour = 12;
	xs.Minute = 44;
	xs.DayInWeek = 0;
	xs.Second = 33.997;
	CommonTimeToGPSTime(&xs, &xsgps);
	GPSTimeToMjdtime(&xsgps, &xsmjd);*/

	RawData rawdata;
	XYZ myposxyz;
	SVPositinAndSpeed POVOFSV;
	
	
	XYZdot myxyzdot;
	FILE *Fin;
	Fin = fopen("20150207.bin", "rb");

	FILE *fw;
	fw = fopen("result20150207.txt", "w+");
	fprintf(fw, "TIME.SecOfWeek   blh0.B        blh0.L        blh0.H      X       Y        Z       Dop       sigma0     mpos  \n");
	
	//FILE *RTCMFile;
	//RTCMFile = fopen("20160620.bin.rtcm", "rb");

	double Dop = 0.0;
	double sigma0 = 0.0;
	double mpos = Dop*sigma0;
	while (!feof(Fin))
	{
		if (ReadOmestarBinFile(Fin, &rawdata) == 43)
		{
			int GPSEPHEMSVCanBeUsednum = 0;
			for (int i = 0; i < rawdata.Obsnum; i++)
			{
				if (GPSEPHEMCanBeUsed(&rawdata,rawdata.Obs[i].PRN, i))
				{
					GPSEPHEMSVCanBeUsednum++;
				}
			}

			//定位模块，因为读到了观测值数据。判断是不是有星历，有就进行定位;
			if (GPSEPHEMSVCanBeUsednum > 3)
			{
				
				if (rawdata.Obs->TIME.SecOfWeek == 562142)
				{
					PositioningAndSpeeingFirst(&rawdata, GPSEPHEMSVCanBeUsednum, &myposxyz, &myxyzdot,&Dop,&sigma0);
					mpos = Dop*sigma0;
				}
				else
				{
					PositioningAndSpeeingFirst(&rawdata, GPSEPHEMSVCanBeUsednum, &myposxyz, &myxyzdot, &Dop, &sigma0);
					mpos = Dop*sigma0;
				}
			}
				BLH blh0;
				XYZToBLH(&myposxyz, &blh0);
				blh0.B = blh0.B * 180 / PI;
				blh0.L = blh0.L * 180 / PI;
				fprintf(fw, "%lf  %.10lf  %.10lf  %.10lf  %.10f  %.10f  %.10f  %8.4f  %8.4f  %8.4f  \n", rawdata.Obs->TIME.SecOfWeek, blh0.B, blh0.L, blh0.H, myposxyz.X, myposxyz.Y, myposxyz.Z, Dop, sigma0, mpos);
				myxyzdot.Xdot = 0.0; myxyzdot.Ydot = 0.0; myxyzdot.Zdot = 0.0;//必须每次将速度更新至0否则每次会累加;
			}
		}

	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}