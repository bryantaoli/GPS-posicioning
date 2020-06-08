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
		GT->SecOfWeek = (MJD->Days + MJD->FracDay - 44244 - GT->Week * 7) * 86400;
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
		if (MJD->FracDay < 0) {
			MJD->Days = MJD->Days - 1;
			MJD->FracDay = MJD->FracDay + 1;
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
		int aa = int(MJD->Days + MJD->FracDay + 2400000.5 + 0.5);
		int bb = aa + 1537;
		int cc = int((bb - 122.1) / 365.25);
		int dd = int(365.25*cc);
		int ee = int((bb - dd) / 30.6001);
		double D=bb - dd - int(30.6001*ee) + MJD->Days + MJD->FracDay + 2400000.5 + 0.5 - aa;
		CT->Day = int(D);
		double H=(D-CT->Day)*24;
		CT->Hour = int(H);
		double M = (H - CT->Hour)*60;
		CT->Minute = int(M);
		CT->Second = (M - CT->Minute) * 60;
		CT->Month = ee - 1 - 12 * int(ee / 14);
		CT->Year = cc - 4715 - int((7 + CT->Month) / 10);
		CT->DayInWeek = aa % 7+1;
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
