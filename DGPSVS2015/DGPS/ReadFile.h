#pragma once
#include"Datadecoding.h"
#include"Referencechange.h"
#include"Timechange.h"


//每个卫星对应的时间不一样，所以要把时间放RTCMBody里;

struct RTCMHeader
{
	unsigned short RTCMType;
	unsigned int BaseStationID;
	unsigned int Zcount;
	unsigned short LenOfMessage;
	unsigned char StateOfWork;
	//初始化结构体的意义，如果不初始化，下次再定义的时候他的值是不会变的：
	//当然，也可反向思维，如果要用到每次定义它值不变的时候，就不初始化？;
	//不对！要值不变直接把这个变量放这块外面不行了？;还多此一举不初始化干嘛;
	RTCMHeader()
	{
		RTCMType = 52428;
		BaseStationID = 3435973836;
		Zcount = 3435973836;
		LenOfMessage = 52428;
		StateOfWork = 204;
	}
};

struct RTCM1body
{
	unsigned short s;
	double ScaleFactorForPRC;
	double ScaleFactorForRRC;
	unsigned short UDRE;
	double PRC;
	short PRCwithoutScale;
	double RRC;
	short RRCwithoutScale;
	unsigned short AOD;
	double TimeOfHour;
	RTCM1body()
	{
		s = 52428;
		ScaleFactorForPRC = -9.2559631349317831e+61;
		ScaleFactorForRRC = -9.2559631349317831e+61;
		UDRE = 52428;
		PRC = -9.2559631349317831e+61;
		PRCwithoutScale = -13108;
		RRC= -9.2559631349317831e+61;
		RRCwithoutScale = -13108;
		AOD = 52428;
		TimeOfHour= -9.2559631349317831e+61;
	}
};

struct RTCM1
{
	//注意，结构体套结构体的时候，初始化结构体必须调用内层的构造函数;
	RTCMHeader header;
	RTCM1body rtcm1body[32];
	RTCM1()
	{
		RTCMHeader();
		RTCM1body();
	}
};








bool ReadRTCMFile(FILE *RTCMFile, RTCM1 *R, unsigned char *d29last, unsigned char *d30last);
//bool ReadRTCMFile(FILE *RTCMFile,FILE*RTCMFilein, RTCM1 *R, unsigned char *d29last, unsigned char *d30last);
bool ReadFiveByte(FILE *RTCMFile, RTCM1 *R, unsigned char *d29last, unsigned char *d30last,unsigned char* Combined);
bool OEC(unsigned char* OECRec,unsigned char* ByteAfterRoll, unsigned char *d29last, unsigned char *d30last);
bool ReadOnePage(FILE *RTCMFile, RTCM1 *R, unsigned char *d29last, unsigned char *d30last);

bool RefreshRTCM1(RTCM1 *R);

bool DecodeRTCMFirstSV(unsigned char* Combined1, unsigned char* Combined2, RTCM1 *R_temp);

bool DecodeRTCMSecondSV(unsigned char* Combined2, unsigned char* Combined3, unsigned char* Combined4, RTCM1 *R_temp);
bool DecodeRTCMThirdSV(unsigned char* Combined4, unsigned char* Combined5, RTCM1 *R_temp);
//bool MakeRTCMAndOBSsametime(RTCM1 *R1, RawData *rawdata);
