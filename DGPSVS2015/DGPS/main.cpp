#include<stdio.h>
#include"Datadecoding.h"
#include"Timechange.h"
#include"Referencechange.h"
#include "Positioning.h"
#include <stdlib.h>
#include "ReadFile.h"
#include <string.h>
#include <math.h>
#define CRTDBG_MAP_ALLOC  
#include <crtdbg.h>
#pragma warning(disable:4996)

bool IsRTCMFasterThanOBS(RTCM1* R1, RawData* rawdata);

int main()
{
	//_CrtSetBreakAlloc(5610659);
	RawData rawdata;
	XYZ myposxyz; SVPositinAndSpeed POVOFSV; XYZdot myxyzdot;

	FILE *Fin;
	Fin = fopen("20160620.bin", "rb");

	FILE *DGPS;
	DGPS = fopen("RTCMresult20160620.txt", "w+");
	fprintf(DGPS, "TIME.SecOfWeek   blh0.B        blh0.L        blh0.H             X          Y           Z             VX       VY        VZ       Dop       sigma0     mpos  \n");

	FILE *RTCMFile;
	RTCMFile = fopen("20160620.bin.rtcm", "rb");

	FILE *RTCMtxt;
	RTCMtxt = fopen("RTCM20160620.txt", "w+");

	FILE *SPP;
	SPP = fopen("SPP20160620.txt", "w+");
	fprintf(SPP, "TIME.SecOfWeek   blh0.B        blh0.L        blh0.H             X          Y           Z                   VX       VY        VZ       Dop       sigma0     mpos  \n");
	//fprintf(RTCMtxtin, "PRN     TimeOfHour   PRC       RRC       AOD     \n");

	RTCM1 R;

	unsigned char d29last = 0; unsigned char d30last = 1;

	double Dop = 0.0; double sigma0 = 0.0; double mpos = Dop*sigma0;
	while ((!feof(Fin))&&(!feof(RTCMFile)))
	{
		if (ReadOmestarBinFile(Fin, &rawdata) == 43)
		{
			RTCM1 R1;
			//解文件的时候差分数据和观测数据共同卫星不同步，怎么办，没法定位;

			//根据本文件两者头的AOD不对齐做的处理，找到AOD对齐为之;
			if (rawdata.Obs[0].TIME.SecOfWeek == 196024)
			{
				ReadRTCMFile(RTCMFile, &R1, &d29last, &d30last);
				
				do
				{
					//ReadOmestarBinFile(Fin, &rawdata);
					ReadRTCMFile(RTCMFile, &R1, &d29last, &d30last);
					if (R1.rtcm1body[0].TimeOfHour==1629.00)
					{
						double trsada = 1;
					}
				} while (rawdata.GPSEPH[0].IODE1 != R1.rtcm1body[0].AOD);
				R = R1;
				for (int i = 0; i < 160; i++)
				{
					ReadOmestarBinFile(Fin, &rawdata);
				}
			}
			else 
			{
				if ((IsRTCMFasterThanOBS(&R, &rawdata)) == false)
				{
					/*do
					{
						ReadRTCMFile(RTCMFile, &R1, &d29last, &d30last);
					} while ((IsRTCMFasterThanOBS(&R1, &rawdata)) == true);*/
					while (1)
					{
						ReadRTCMFile(RTCMFile, &R1, &d29last, &d30last);
						if ((IsRTCMFasterThanOBS(&R1, &rawdata)) == true)
						{
							break;
						}
					}
					R = R1;
				}
			}
			for (int i = 0; i < 32; i++)
			{
				if (R1.rtcm1body[i].s == 0)
				{
					fprintf(RTCMtxt, "%2d    %lf  %.8lf  %.8lf  %d   \n", i + 1, R1.rtcm1body[i].TimeOfHour, R1.rtcm1body[i].PRC, R1.rtcm1body[i].RRC, R1.rtcm1body[i].AOD);
				}
			}
			fprintf(RTCMtxt, "\n");

			//ReadRTCMFile(RTCMFile, RTCMtxtin, &R1, &d29last, &d30last);

			//R1 = R2;

			int SVEPHEMAndRTCMCanBeUsednum = 0;
			
			//AOD和IODE只差1是什么情况;
			for (int i = 0; i < rawdata.Obsnum; i++)
			{
				if (GPSEPHEMCanBeUsed(&rawdata, rawdata.Obs[i].PRN, i))
				{
					if (RTCMCanBeUsed(&R, rawdata.Obs[i].PRN, &rawdata))
					{
						SVEPHEMAndRTCMCanBeUsednum++;
					}
				}
			}
			if (rawdata.Obs[0].TIME.SecOfWeek == 260105.000000)
			{
				double jasfj = 1;
			}
			if (SVEPHEMAndRTCMCanBeUsednum > 3)
			{
				PVWithRTD(&rawdata, &R, SVEPHEMAndRTCMCanBeUsednum, &myposxyz, &myxyzdot, &Dop, &sigma0);
				mpos = Dop*sigma0;
				BLH blh0;
				XYZToBLH(&myposxyz, &blh0);
				blh0.B = blh0.B * 180 / PI;
				blh0.L = blh0.L * 180 / PI;
				fprintf(DGPS, "%lf  %.8lf  %.8lf  %lf  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  \n", rawdata.Obs->TIME.SecOfWeek, blh0.B, blh0.L, blh0.H, myposxyz.X, myposxyz.Y, myposxyz.Z, myxyzdot.Xdot, myxyzdot.Ydot, myxyzdot.Zdot, Dop, sigma0, mpos);
				printf("%lf  \n", rawdata.Obs->TIME.SecOfWeek);
				//fprintf(DGPS, "%3d  \n", SVEPHEMAndRTCMCanBeUsednum);
				myxyzdot.Xdot = 0.0; myxyzdot.Ydot = 0.0; myxyzdot.Zdot = 0.0;//必须每次将速度更新至0否则每次会累加;
			}
			if (rawdata.Obs[0].TIME.SecOfWeek == 260172)
			{
				_CrtDumpMemoryLeaks();
				return 0;
			}
			int GPSEPHEMCanBeUsednum = 0;
			for (int i = 0; i < rawdata.Obsnum; i++)
			{
				if (GPSEPHEMCanBeUsed(&rawdata, rawdata.Obs[i].PRN, i))
				{
					GPSEPHEMCanBeUsednum++;
				}
			}
			if (GPSEPHEMCanBeUsednum > 3)
			{
				PositioningAndSpeeingFirst(&rawdata, GPSEPHEMCanBeUsednum, &myposxyz, &myxyzdot, &Dop, &sigma0);
				mpos = Dop*sigma0;
				BLH blh0;
				XYZToBLH(&myposxyz, &blh0);
				blh0.B = blh0.B * 180 / PI;
				blh0.L = blh0.L * 180 / PI;
				fprintf(SPP, "%lf  %.8lf  %.8lf  %lf  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  %8.4f  \n", rawdata.Obs->TIME.SecOfWeek, blh0.B, blh0.L, blh0.H, myposxyz.X, myposxyz.Y, myposxyz.Z, myxyzdot.Xdot, myxyzdot.Ydot, myxyzdot.Zdot, Dop, sigma0, mpos);
				//printf("%lf  \n", rawdata.Obs->TIME.SecOfWeek);
				//fprintf(DGPS, "%3d  \n", SVEPHEMAndRTCMCanBeUsednum);
				myxyzdot.Xdot = 0.0; myxyzdot.Ydot = 0.0; myxyzdot.Zdot = 0.0;//必须每次将速度更新至0否则每次会累加;
			}
		}
	}
	fclose(Fin);
//	fclose(DGPS);
	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}



/*判断RTCM是不是比观测值快，因为头文件包含的原因放不到别的函数里去了就写主函数下面*/
bool IsRTCMFasterThanOBS(RTCM1* R1, RawData* rawdata)
{
	if (fabs(R1->header.Zcount*0.6 - (int(rawdata->Obs[0].TIME.SecOfWeek) % 3600)) > 1800)//说明跳小时了;
	{
		if (R1->header.Zcount*0.6 < (int(rawdata->Obs[0].TIME.SecOfWeek) % 3600))//跳小时的情况下rtcm小 说明rtcm在先;
		{
			return true;
		}
		else  //跳小时的情况下rtcm大 说明rtcm在后;
		{
			return false;
		}
	}
	else
	{
		if (R1->header.Zcount*0.6 > (int(rawdata->Obs[0].TIME.SecOfWeek) % 3600))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
