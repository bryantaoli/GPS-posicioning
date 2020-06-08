#include "Timechange.h"
#include<stdio.h>

bool GPSTimeToMjdtime(GPSTIME* GT, MJDTIME* MJD)
{
	if (GT!=NULL&&MJD!= NULL)
	{
		MJD->Days = int(44244 + 7 * GT->Week + GT->SecOfWeek / 86400);
		MJD->FracDay = 44244 + 7 * GT->Week + GT->SecOfWeek / 86400 - MJD->Days;
		return true;
	}
	else
	{
		return false;
	}
}

bool MjdtimeToGPSTime(MJDTIME * MJD, GPSTIME * GT)
{
	if (MJD!=NULL&&GT!=NULL)
	{
		GT->Week = int((MJD->Days + MJD->FracDay - 44244) / 7);
		double GTSecOfWeek = (MJD->Days + MJD->FracDay - 44244 - GT->Week * 7) * 86400;
		double temp = MJD->Days - 44244 - GT->Week * 7;
		GT->SecOfWeek = (temp + MJD->FracDay) * 86400;
		return true;
	}
	else 
	{
		return false;
	}
}

/*******************************************
通用时转简化儒略时
传入参数（CT的地址和MJD的地址）

********************************************/
bool CommonTimeToMjdtime(CommonTime* CT, MJDTIME* MJD)
{

	if (CT != NULL&& MJD != NULL)
	{
		if (CT->Month < 3)
		{
			MJD->Days = int(int(365.25*(CT->Year - 1))+int(30.6001*(CT->Month + 13))+
			   CT->Day+(CT->Hour+CT->Minute/60.0+CT->Second/3600.0)/24+1720981.5-2400000.5);
			MJD->FracDay = int(365.25*(CT->Year-1)) + int(30.6001*(CT->Month + 13)) +
				CT->Day + (CT->Hour + CT->Minute / 60.0 + CT->Second / 3600.0) / 24 + 1720981.5 - 2400000.5 - MJD->Days;
		}
		else if(CT->Month>2&& CT->Month<13)
		{
			MJD->Days = int(int(365.25*CT->Year + int(30.6001*(CT->Month+1)) +
				CT->Day + (CT->Hour + CT->Minute / 60.0 + CT->Second / 3600.0) / 24 + 1720981.5 - 2400000.5));
			MJD->FracDay = int(365.25*CT->Year) + int(30.6001*(CT->Month + 1)) +
				CT->Day + (CT->Hour + CT->Minute / 60.0 + CT->Second / 3600.0) / 24 + 1720981.5 - 2400000.5 - MJD->Days;
		}
		else
		{
			printf("月份不对啊,按回车键退出");
			getchar();
			exit(1);
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MjdtimeToCommonTime(MJDTIME* MJD, CommonTime*CT)
{
	if (MJD != NULL&&CT!=NULL)
	{
		int a = int(MJD->Days + MJD->FracDay + 2400000.5 + 0.5);
		int b = a + 1537;
		int c = int((b - 122.1) / 365.25);
		int d = int(365.25*c);
		int e = int((b - d) / 30.6001);
		double D=b - d - int(30.6001*e) + MJD->Days + MJD->FracDay + 2400000.5 + 0.5 - a;
		CT->Day = int(D);
		double H=(D-CT->Day)*24;
		CT->Hour = int(H);
		double M = (H - CT->Hour)*60;
		CT->Minute = int(M);
		CT->Second = (M - CT->Minute) * 60;
		CT->Month = e - 1 - 12 * int(e / 14);
		CT->Year = c - 4715 - int((7 + CT->Month) / 10);
		CT->DayInWeek = a % 7+1;
		return true;
	}
	else
	{
		return false;
	}
}

bool CommonTimeToGPSTime(CommonTime * CT, GPSTIME * GT)
{
	MJDTIME MJD;
	CommonTimeToMjdtime(CT,&MJD);
	MjdtimeToGPSTime(&MJD, GT);
	return true;
}

bool GPSTimeToCommonTime(GPSTIME * GT, CommonTime * CT)
{
	MJDTIME MJD;
	GPSTimeToMjdtime(GT, &MJD);
	MjdtimeToCommonTime(&MJD, CT);
	return true;
}
