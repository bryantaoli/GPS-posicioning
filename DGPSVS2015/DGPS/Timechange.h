#pragma once
#include<stdio.h>
#include<stdlib.h>
#ifndef _TIMECHANGE_H
#define _TIMECHANGE_H
struct GPSTIME
{
	unsigned short Week=0;
	double SecOfWeek=0.0;
};
struct MJDTIME
{
	//这么分开的原因是增加位数
	int Days;
	long double FracDay;
	MJDTIME()
	{
		int Days=0;
		long double FracDay=0.0;
	}
};

//这里的通用时必须用的是格林尼治天文台的时间，而不能用北京时间
struct CommonTime
{
	unsigned short Year;
	unsigned short Month;
	unsigned short Day;
	unsigned short Hour;
	unsigned short Minute;
	unsigned short DayInWeek;
	double Second;
};


bool GPSTimeToCommonTime(GPSTIME* GT, CommonTime*CT);
bool GPSTimeToMjdtime(GPSTIME* GT,MJDTIME* MJD);
bool MjdtimeToGPSTime(MJDTIME*MJD,GPSTIME*GT);
bool MjdtimeToCommonTime(MJDTIME* MJD, CommonTime*CT);
bool CommonTimeToGPSTime(CommonTime*CT, GPSTIME* GT);
bool CommonTimeToMjdtime(CommonTime*CT, MJDTIME*MJD);
#endif // !_TIMECHANGE_H
